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

#include "Rendering/FrameGraph/GLRenderScene.hpp"

#include "Foundation/GLUtils.hpp"
#include "SceneGraph/Camera.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/GLFetchSceneRenderState.hpp"

using namespace crimild::gl;
using namespace crimild::gl::framegraph;

RenderScene::RenderScene( const Extent2D &extent ) noexcept
    : Node( "RenderScene" ),
      m_extent( extent )
{
    // TODO
}

RenderScene::~RenderScene( void ) noexcept
{
    // no-op
}

void RenderScene::execute( Context &ctx ) noexcept
{
    auto scene = Simulation::getInstance()->getScene();
    m_renderState =
        scene != nullptr
            ? scene->perform< FetchSceneRenderState >()
            : SceneRenderState {};

    auto camera = Simulation::getInstance()->getMainCamera();
    if ( camera != nullptr ) {
        // Set correct aspect ratio for camera before rendering
        camera->setAspectRatio( m_extent.width / m_extent.height );
    } else {
        return;
    }

    glViewport( 0, 0, m_extent.width, m_extent.height );
    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT );

    for ( auto &[ material, primitives ] : m_renderState.unlitRenderables ) {
        for ( auto &[ primitive, renderables ] : primitives ) {
            for ( auto &renderable : renderables ) {
                // bindMaterial( material.get() );

                // cmds->bindPipeline( m_resources.materials[ material.get() ].pipeline );
                // cmds->bindDescriptorSet( 0, m_resources.renderPass.descriptorSet );
                // cmds->bindDescriptorSet( 1, m_resources.materials[ material.get() ].descriptorSet );

                // // Vulkan spec only requires a minimum of 128 bytes. Anything larger should
                // // use normal uniforms instead.
                // cmds->pushConstants( VK_SHADER_STAGE_VERTEX_BIT, 0, renderable );

                // cmds->drawPrimitive( primitive );
            }
        }
    }
}
