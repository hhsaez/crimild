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

#include "Rendering/RenderPasses/VulkanDebugAttachmentPass.hpp"

#include "Rendering/RenderPasses/VulkanClearPass.hpp"
#include "Rendering/RenderPasses/VulkanShaderPass.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "SceneGraph/Camera.hpp"

using namespace crimild;
using namespace crimild::vulkan;

DebugAttachmentPass::DebugAttachmentPass(
    RenderDevice *renderDevice,
    std::string_view name,
    const FramebufferAttachment *inputAttachment,
    std::string_view mode
) noexcept
    : RenderPassBase( renderDevice ),
      m_inputAttachment( inputAttachment ),
      m_uniforms( crimild::alloc< UniformBuffer >( Vector2f { 0, 1 } ) )
{
    m_colorAttachment.name = name;

    if ( m_uniforms != nullptr ) {
        getRenderDevice()->bind( m_uniforms.get() );
    }

    init();

    m_clear = crimild::alloc< ClearPass >(
        renderDevice,
        std::vector< const FramebufferAttachment * > {
            &m_colorAttachment,
        }
    );

    const std::string shaderSource = [ & ] {
        if ( mode == "depth" ) {
            return R"(
                layout ( set = 0, binding = 0 ) uniform Camera {
                    float near;
                    float far;
                } uCamera;

                layout ( set = 0, binding = 1 ) uniform sampler2D uInput;

                float linearizeDepth( float depth )
                {
                    float n = uCamera.near;
                    float f = uCamera.far;
                    float z = depth;
                    return ( 2.0 * n ) / ( f + n - z * ( f - n ) );
                }

                void main()
                {
                    float depth = texture( uInput, inTexCoord ).r;
                    outColor = vec4( vec3( linearizeDepth( depth ) ), 1.0);
                }
            )";
        } else if ( mode == "rgb" ) {
            return R"(
                layout ( set = 0, binding = 0 ) uniform sampler2D uInput;

                void main()
                {
                    vec3 color = texture( uInput, inTexCoord ).rgb;
                    outColor = vec4( color, 1.0 );
                }
            )";
        } else if ( mode == "r" ) {
            return R"(
                layout ( set = 0, binding = 0 ) uniform sampler2D uInput;

                void main()
                {
                    float color = texture( uInput, inTexCoord ).r;
                    outColor = vec4( vec3( color ), 1.0 );
                }
            )";
        } else if ( mode == "g" ) {
            return R"(
                layout ( set = 0, binding = 0 ) uniform sampler2D uInput;

                void main()
                {
                    float color = texture( uInput, inTexCoord ).g;
                    outColor = vec4( vec3( color ), 1.0 );
                }
            )";
        } else if ( mode == "b" ) {
            return R"(
                layout ( set = 0, binding = 0 ) uniform sampler2D uInput;

                void main()
                {
                    float color = texture( uInput, inTexCoord ).b;
                    outColor = vec4( vec3( color ), 1.0 );
                }
            )";
        } else {
            return R"(
                layout ( set = 0, binding = 0 ) uniform sampler2D uInput;

                void main()
                {
                    outColor = texture( uInput, inTexCoord );
                }
            )";
        }
    }();

    m_shader = crimild::alloc< vulkan::ShaderPass >(
        renderDevice,
        shaderSource,
        &m_colorAttachment,
        mode == "depth" ? m_uniforms : nullptr,
        std::vector< const FramebufferAttachment * > { m_inputAttachment }
    );
}

DebugAttachmentPass::~DebugAttachmentPass( void ) noexcept
{
    m_clear = nullptr;
    m_shader = nullptr;

    deinit();
}

Event DebugAttachmentPass::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE: {
            deinit();
            init();
            break;
        }

        default:
            break;
    }

    m_clear->handle( e );
    m_shader->handle( e );

    return e;
}

void DebugAttachmentPass::render( Node *, Camera *camera ) noexcept
{
    m_uniforms->setValue( Vector2 { camera->getNear(), camera->getFar() } );
    getRenderDevice()->update( m_uniforms.get() );

    m_clear->render();
    m_shader->render();

    getRenderDevice()->flush( m_colorAttachment );
}

void DebugAttachmentPass::init( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    m_renderArea = VkRect2D {
        .offset = { 0, 0 },
        .extent = m_inputAttachment->extent,
    };

    const auto extent = m_renderArea.extent;

    getRenderDevice()->createFramebufferAttachment( m_colorAttachment.name, extent, VK_FORMAT_R8G8B8A8_UNORM, m_colorAttachment );
}

void DebugAttachmentPass::deinit( void ) noexcept
{
    CRIMILD_LOG_TRACE();

    getRenderDevice()->destroyFramebufferAttachment( m_colorAttachment );
}
