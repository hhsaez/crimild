/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Rendering/RenderPasses/VulkanScenePass.hpp"

#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;
using namespace crimild::vulkan;

ScenePass::ScenePass( RenderDevice *renderDevice ) noexcept
    : m_renderDevice( renderDevice ),
      m_shadowPass( renderDevice ),
      m_gBufferPass( renderDevice ),
      m_localLightingPass(
          renderDevice,
          m_gBufferPass.getAlbedoAttachment(),
          m_gBufferPass.getPositionAttachment(),
          m_gBufferPass.getNormalAttachment(),
          m_gBufferPass.getMaterialAttachment(),
          m_shadowPass.getShadowAttachment() ),
      m_skyboxPass(
          renderDevice,
          m_localLightingPass.getColorAttachment(),
          m_gBufferPass.getDepthStencilAttachment() )
{
    // no-op
}

Event ScenePass::handle( const Event &e ) noexcept
{
    m_shadowPass.handle( e );
    m_gBufferPass.handle( e );
    m_localLightingPass.handle( e );
    m_skyboxPass.handle( e );

    return e;
}

void ScenePass::render( Node *scene, Camera *camera ) noexcept
{
    // TODO: maybe we should use the ClearPass and have ScenePass create its own color/depth attachments
    // instead of relying on inner passes to do that.

    // TODO: Fetch renderables here so the scene is traverse only once and not multiple times (one
    // for each render pass below).

    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    m_shadowPass.render( scene, camera );
    m_gBufferPass.render( scene, camera );

    auto transitionAttachment = [ & ]( const auto att ) {
        getRenderDevice()->transitionImageLayout(
            commandBuffer,
            att->image,
            att->format,
            getRenderDevice()->formatIsColor( att->format )
                ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            getRenderDevice()->formatIsColor( att->format )
                ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            att->mipLevels,
            att->layerCount );
    };

    const auto attachments = std::vector< const vulkan::FramebufferAttachment * > {
        m_gBufferPass.getAlbedoAttachment(),
        m_gBufferPass.getPositionAttachment(),
        m_gBufferPass.getNormalAttachment(),
        m_gBufferPass.getMaterialAttachment(),
    };

    for ( const auto att : attachments ) {
        transitionAttachment( att );
    }

    m_localLightingPass.render( scene, camera );
    m_skyboxPass.render( scene, camera );

    // Accumulated color buffer can be transitioned now
    transitionAttachment( m_localLightingPass.getColorAttachment() );

    // Depth buffer can be transition now
    transitionAttachment( m_gBufferPass.getDepthStencilAttachment() );
}
