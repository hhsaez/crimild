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

#include "Rendering/Compositions/DebugComposition.hpp"

#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Texture.hpp"

namespace crimild {

    namespace compositions {

        namespace utils {

            class DebugInput : public SharedObject {
            };

        }

    }

}

using namespace crimild;
using namespace crimild::compositions;

Composition crimild::compositions::debug( Composition cmp ) noexcept
{
    static auto withDimensions = []( const Rectf &dimensions ) {
        return ViewportDimensions {
            .scalingMode = ScalingMode::SWAPCHAIN_RELATIVE,
            .dimensions = dimensions,
        };
    };

    static auto withPadding = []( const ViewportDimensions &viewport ) {
        const auto padding = 0.0125f;
        return ViewportDimensions {
            .scalingMode = viewport.scalingMode,
            .dimensions = Rectf(
                viewport.dimensions.getX() + padding,
                viewport.dimensions.getY() + padding,
                viewport.dimensions.getWidth() - 2.0f * padding,
                viewport.dimensions.getHeight() - 2.0f * padding ),
        };
    };

    Array< ViewportDimensions > viewports = {
        // main attachment
        withDimensions( Rectf( 0.0f, 0.0f, 1.0f, 1.0f ) ),

        // right column
        withPadding( withDimensions( Rectf( 0.8f, 0.8f, 0.2f, 0.2f ) ) ),
        withPadding( withDimensions( Rectf( 0.8f, 0.6f, 0.2f, 0.2f ) ) ),
        withPadding( withDimensions( Rectf( 0.8f, 0.4f, 0.2f, 0.2f ) ) ),
        withPadding( withDimensions( Rectf( 0.8f, 0.2f, 0.2f, 0.2f ) ) ),
        withPadding( withDimensions( Rectf( 0.8f, 0.0f, 0.2f, 0.2f ) ) ),

        // left column
        withPadding( withDimensions( Rectf( 0.0f, 0.8f, 0.2f, 0.2f ) ) ),
        withPadding( withDimensions( Rectf( 0.0f, 0.6f, 0.2f, 0.2f ) ) ),
        withPadding( withDimensions( Rectf( 0.0f, 0.4f, 0.2f, 0.2f ) ) ),
        withPadding( withDimensions( Rectf( 0.0f, 0.2f, 0.2f, 0.2f ) ) ),
        withPadding( withDimensions( Rectf( 0.0f, 0.0f, 0.2f, 0.2f ) ) ),
    };

    /*
				auto uniforms = [&] {
					auto formatIsDepthStencil = [&] {
						auto format = input->format;
						switch ( format ) {
							case Format::DEPTH_16_UNORM:
							case Format::DEPTH_32_SFLOAT:
							case Format::DEPTH_16_UNORM_STENCIL_8_UINT:
							case Format::DEPTH_24_UNORM_STENCIL_8_UINT:
							case Format::DEPTH_32_SFLOAT_STENCIL_8_UINT:
							case Format::DEPTH_STENCIL_DEVICE_OPTIMAL:
								return true;
							default:
								return false;
						}
					}();
					return crimild::alloc< UniformBuffer >(
						Uniforms {
							.attachmentType = formatIsDepthStencil ? 1 : 0,
						}
					);
    */

    auto renderPass = cmp.create< RenderPass >();
    renderPass->attachments = {
        [ & ] {
            auto att = cmp.createAttachment( "debug" );
            att->usage = Attachment::Usage::COLOR_ATTACHMENT;
            att->format = Format::R8G8B8A8_UNORM;
            att->imageView = crimild::alloc< ImageView >();
            att->imageView->image = crimild::alloc< Image >();
            return crimild::retain( att );
        }(),
    };

    auto pipeline = cmp.create< GraphicsPipeline >();
    pipeline->setProgram(
        [ & ] {
            auto program = crimild::alloc< ShaderProgram >();
            program->setShaders(
                {
                    crimild::alloc< Shader >(
                        Shader::Stage::VERTEX,
                        CRIMILD_TO_STRING(
                            vec2 positions[ 6 ] = vec2[](
                                vec2( -1.0, 1.0 ),
                                vec2( -1.0, -1.0 ),
                                vec2( 1.0, -1.0 ),

                                vec2( -1.0, 1.0 ),
                                vec2( 1.0, -1.0 ),
                                vec2( 1.0, 1.0 ) );

                            vec2 texCoords[ 6 ] = vec2[](
                                vec2( 0.0, 0.0 ),
                                vec2( 0.0, 1.0 ),
                                vec2( 1.0, 1.0 ),

                                vec2( 0.0, 0.0 ),
                                vec2( 1.0, 1.0 ),
                                vec2( 1.0, 0.0 ) );

                            layout( location = 0 ) out vec2 outTexCoord;

                            void main() {
                                gl_Position = vec4( positions[ gl_VertexIndex ], 0.0, 1.0 );
                                outTexCoord = texCoords[ gl_VertexIndex ];
                            } ) ),
                        crimild::alloc< Shader >(
                            Shader::Stage::FRAGMENT,
                            CRIMILD_TO_STRING(
                                layout( location = 0 ) in vec2 inTexCoord;

                                layout( set = 0, binding = 0 ) uniform sampler2D uColorMap;

                                layout( location = 0 ) out vec4 outColor;

                                void main() {
                                    vec4 color = texture( uColorMap, inTexCoord );
                                    outColor = vec4( color.rgb, 1.0 );
                                } ) ),
                } );
            program->descriptorSetLayouts = {
                [] {
                    auto layout = crimild::alloc< DescriptorSetLayout >();
                    layout->bindings = {
                        {
                            .descriptorType = DescriptorType::TEXTURE,
                            .stage = Shader::Stage::FRAGMENT,
                        },
                    };
                    return layout;
                }(),
            };
            return program;
        }() );
    pipeline->viewport = { .scalingMode = ScalingMode::DYNAMIC };
    pipeline->scissor = { .scalingMode = ScalingMode::DYNAMIC };

    auto recordAttachmentCommands = [ &, index = 0 ]( auto commandBuffer, auto att ) mutable {
        if ( att == crimild::get_ptr( renderPass->attachments[ 0 ] ) ) {
            // Ignore attachments created in this composition
            return;
        }

        if ( index == viewports.size() ) {
            CRIMILD_LOG_WARNING( "Cannot record attachments. Insufficient viewports" );
            return;
        }

        auto imageView = att->imageView;
        if ( imageView == nullptr ) {
            return;
        }

        auto viewport = viewports[ index++ ];
        commandBuffer->setViewport( viewport );
        commandBuffer->setScissor( viewport );

        auto descriptors = [ & ] {
            auto descriptorSet = cmp.create< DescriptorSet >();
            descriptorSet->descriptors = {
                Descriptor {
                    .descriptorType = DescriptorType::TEXTURE,
                    .obj = [ & ] {
                        auto texture = cmp.create< Texture >();
                        texture->imageView = imageView;
                        texture->sampler = [] {
                            auto sampler = crimild::alloc< Sampler >();
                            sampler->setMinFilter( Sampler::Filter::NEAREST );
                            sampler->setMagFilter( Sampler::Filter::NEAREST );
                            return sampler;
                        }();
                        return crimild::retain( texture );
                    }() },
            };
            return descriptorSet;
        }();
        commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
        commandBuffer->bindDescriptorSet( crimild::get_ptr( descriptors ) );

        commandBuffer->draw( 6 );
    };

    auto mainAttachment = cmp.getOutput();

    auto commands = cmp.create< CommandBuffer >();
    commands->begin( CommandBuffer::Usage::SIMULTANEOUS_USE );
    commands->beginRenderPass( renderPass, nullptr );
    recordAttachmentCommands( commands, mainAttachment );
    cmp.eachAttachment(
        [ & ]( auto att ) mutable {
            if ( att != mainAttachment ) {
                // Render additional attachments
                recordAttachmentCommands( commands, att );
            }
        } );
    commands->endRenderPass( renderPass );
    commands->end();

    renderPass->setCommandRecorder(
        [ commands ] {
            return commands;
        } );

    cmp.setOutput( crimild::get_ptr( renderPass->attachments[ 0 ] ) );

    return cmp;
}
