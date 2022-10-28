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

#include "Rendering/RenderPasses/VulkanClearPass.hpp"
#include "Rendering/RenderPasses/VulkanGBufferPass.hpp"
#include "Rendering/RenderPasses/VulkanLocalLightingPass.hpp"
#include "Rendering/RenderPasses/VulkanShadowPass.hpp"
#include "Rendering/RenderPasses/VulkanUnlitPass.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "SceneGraph/Camera.hpp"
#include "Visitors/VulkanFetchSceneRenderState.hpp"

using namespace crimild;
using namespace crimild::vulkan;

ScenePass::ScenePass( RenderDevice *renderDevice ) noexcept
    : RenderPassBase( renderDevice )
{
    m_renderArea = VkRect2D {
        .offset = { 0, 0 },
        .extent = { .width = 1024, .height = 1024 },
    };

    // Make composition attachment opaque
    m_attachments.back().clearColor = ColorRGBA { 0, 0, 0, 1 };

    init();

    m_clear = crimild::alloc< ClearPass >(
        renderDevice,
        [ & ] {
            std::vector< const FramebufferAttachment * > ret;
            ret.reserve( m_attachments.size() );
            for ( const auto &att : m_attachments ) {
                ret.push_back( &att );
            }
            return ret;
        }()
    );

    m_shadowPass = crimild::alloc< ShadowPass >( renderDevice );

    m_gBuffer = crimild::alloc< GBufferPass >(
        renderDevice,
        std::vector< const FramebufferAttachment * > {
            &m_attachments[ 0 ],
            &m_attachments[ 1 ],
            &m_attachments[ 2 ],
            &m_attachments[ 3 ],
            &m_attachments[ 4 ],
        }
    );

    m_lighting = crimild::alloc< LocalLightingPass >(
        renderDevice,
        std::vector< const FramebufferAttachment * > {
            &m_attachments[ 1 ],
            &m_attachments[ 2 ],
            &m_attachments[ 3 ],
            &m_attachments[ 4 ],
        },
        &m_attachments.back() // color composition
    );

    m_unlit = crimild::alloc< UnlitPass >(
        renderDevice,
        &m_attachments.back(), // color composition
        &m_attachments.front() // depth
    );
}

ScenePass::~ScenePass( void ) noexcept
{
    m_clear = nullptr;
    m_shadowPass = nullptr;
    m_gBuffer = nullptr;
    m_lighting = nullptr;
    m_unlit = nullptr;

    deinit();
}

Event ScenePass::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE: {
            m_renderArea.extent = {
                .width = uint32_t( e.extent.width ),
                .height = uint32_t( e.extent.height ),
            };
            deinit();
            init();
            break;
        }

        default:
            break;
    }

    m_clear->handle( e );
    m_shadowPass->handle( e );
    m_gBuffer->handle( e );
    m_lighting->handle( e );
    m_unlit->handle( e );

    return e;
}

void ScenePass::render( Node *scene, Camera *camera ) noexcept
{
    m_clear->render();

    auto renderState =
        scene != nullptr
            ? scene->perform< FetchSceneRenderState >()
            : FetchSceneRenderState::Result {};

    if ( camera != nullptr ) {
        // Set correct aspect ratio for camera before rendering
        camera->setAspectRatio( float( m_renderArea.extent.width ) / float( m_renderArea.extent.height ) );
    }

    m_shadowPass->render( renderState.lights, renderState.shadowCasters, camera );

    m_gBuffer->render( renderState.litRenderables, camera );

    // flush all G-Buffer color attachments (depth will be written later) so they
    // can be read in the next passes.
    getRenderDevice()->flush( m_attachments[ 1 ] );
    getRenderDevice()->flush( m_attachments[ 2 ] );
    getRenderDevice()->flush( m_attachments[ 3 ] );
    getRenderDevice()->flush( m_attachments[ 4 ] );

    m_lighting->render( renderState.lights, camera );

    // Render unlit
    m_unlit->render( renderState.unlitRenderables, camera );

    // Render environment
    m_unlit->render( renderState.envRenderables, camera );

    // Finally, flush depth and composition attachments
    getRenderDevice()->flush( m_attachments.front() );
    getRenderDevice()->flush( m_attachments.back() );
}

void ScenePass::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    const auto extent = m_renderArea.extent;

    getRenderDevice()->createFramebufferAttachment( "Scene/Depth", extent, VK_FORMAT_D32_SFLOAT, m_attachments[ 0 ] );
    getRenderDevice()->createFramebufferAttachment( "Scene/Albedo", extent, VK_FORMAT_R32G32B32A32_SFLOAT, m_attachments[ 1 ] );
    getRenderDevice()->createFramebufferAttachment( "Scene/Position", extent, VK_FORMAT_R32G32B32A32_SFLOAT, m_attachments[ 2 ] );
    getRenderDevice()->createFramebufferAttachment( "Scene/Normal", extent, VK_FORMAT_R32G32B32A32_SFLOAT, m_attachments[ 3 ] );
    getRenderDevice()->createFramebufferAttachment( "Scene/Material", extent, VK_FORMAT_R32G32B32A32_SFLOAT, m_attachments[ 4 ] );
    getRenderDevice()->createFramebufferAttachment( "Scene", extent, VK_FORMAT_R32G32B32A32_SFLOAT, m_attachments[ 5 ] );
}

void ScenePass::deinit( void ) noexcept
{
    vkDeviceWaitIdle( getRenderDevice()->getHandle() );
    
    for ( auto &att : m_attachments ) {
        getRenderDevice()->destroyFramebufferAttachment( att );
    }
}
