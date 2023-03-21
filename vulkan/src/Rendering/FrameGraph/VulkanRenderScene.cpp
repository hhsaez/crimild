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
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Rendering/FrameGraph/VulkanRenderScene.hpp"

#include "Rendering/FrameGraph/VulkanComputeSceneLighting.hpp"
#include "Rendering/FrameGraph/VulkanRenderGBuffer.hpp"
#include "Rendering/FrameGraph/VulkanRenderSceneLighting.hpp"
#include "Rendering/FrameGraph/VulkanRenderSceneUnlit.hpp"
#include "Rendering/FrameGraph/VulkanRenderShadowMaps.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderTarget.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Node.hpp"
#include "Visitors/VulkanFetchSceneRenderState.hpp"

using namespace crimild::vulkan;
using namespace crimild::vulkan::framegraph;

RenderScene::RenderScene( RenderDevice *device, const VkExtent2D &extent )
    : RenderBase( device, "RenderScene", extent )
{
    m_depthTarget = crimild::alloc< RenderTarget >( device, "Scene/Depth", VK_FORMAT_D32_SFLOAT, extent );

    auto gBufferTargets = std::vector< std::shared_ptr< RenderTarget > > {
        m_depthTarget,
        crimild::alloc< RenderTarget >( device, "Scene/Albedo", VK_FORMAT_R32G32B32A32_SFLOAT, extent ),
        crimild::alloc< RenderTarget >( device, "Scene/Position", VK_FORMAT_R32G32B32A32_SFLOAT, extent ),
        crimild::alloc< RenderTarget >( device, "Scene/Normal", VK_FORMAT_R32G32B32A32_SFLOAT, extent ),
        crimild::alloc< RenderTarget >( device, "Scene/Material", VK_FORMAT_R32G32B32A32_SFLOAT, extent ),
    };

    m_colorTarget = crimild::alloc< RenderTarget >( device, "Scene/Color", VK_FORMAT_R32G32B32A32_SFLOAT, extent );
    m_colorTarget->setClearValue( VkClearValue { .color = { .float32 = { 0, 0, 0, 1 } } } );

    m_shadows = crimild::alloc< RenderShadowMaps >( device );

    m_gBuffer = crimild::alloc< RenderGBuffer >( device, extent, gBufferTargets );
    // m_gBuffer->invalidates( { m_colorTarget } );
    // m_gBuffer->flushes( gBufferTargets );

    m_lighting = crimild::alloc< RenderSceneLighting >(
        device,
        extent,
        std::vector< std::shared_ptr< RenderTarget > > {
            gBufferTargets[ 1 ],
            gBufferTargets[ 2 ],
            gBufferTargets[ 3 ],
            gBufferTargets[ 4 ],
        },
        m_colorTarget
    );

    m_unlit = crimild::alloc< RenderSceneUnlit >( device, extent, m_depthTarget, m_colorTarget );

    m_environment = crimild::alloc< RenderSceneUnlit >( device, extent, m_depthTarget, m_colorTarget );

    // m_compute = crimild::alloc< ComputeSceneLighting >(
    //     device,
    //     std::vector< std::shared_ptr< RenderTarget > > {
    //         gBufferTargets[ 1 ],
    //         gBufferTargets[ 2 ],
    //         gBufferTargets[ 3 ],
    //         gBufferTargets[ 4 ],
    //     },
    //     m_colorTarget
    // );

    // m_unlit = crimild::alloc< RenderUnlit >();
    // m_unlit->reads( { sceneDepth, sceneColorHDR } );
    // m_unlit->writes( { sceneDepth, sceneColorHDR } );

    // m_tonemapping = crimild::alloc< ComputeTonemapping >();
    // m_tonemapping->reads( { sceneColorHDR } );
    // m_tonemapping->writes( { sceneColor } );
}

void RenderScene::onResize( void ) noexcept
{
    // TODO
}

void RenderScene::render( Node *scene, Camera *camera ) noexcept
{
    const auto renderState =
        scene != nullptr
            ? scene->perform< FetchSceneRenderState >()
            : FetchSceneRenderState::Result {};

    if ( camera != nullptr ) {
        // Set correct aspect ratio for camera before rendering
        camera->setAspectRatio( float( getExtent().width ) / float( getExtent().height ) );
    }

    // m_shadows->execute( {}, renderState, camera, {} );

    // m_gBuffer->execute(
    //     {},
    //     renderState,
    //     camera,
    //     Barriers {
    //         .images = { m_colorTarget->getImage() },
    //     }
    // );

    m_shadows->render( renderState, camera );
    m_gBuffer->render( renderState.litRenderables, camera );
    m_lighting->render( renderState, camera );
    m_unlit->render( renderState.unlitRenderables, camera );
    m_unlit->render( renderState.envRenderables, camera );
    // m_compute->execute();
}
