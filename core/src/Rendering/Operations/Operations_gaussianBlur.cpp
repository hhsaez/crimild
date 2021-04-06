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

#include "Components/MaterialComponent.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Operations/OperationUtils.hpp"
#include "Rendering/Operations/Operations.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Simulation/Simulation.hpp"

namespace crimild {

    namespace framegraph {

        SharedPointer< FrameGraphOperation > gaussianBlur( SharedPointer< FrameGraphResource > const &image, Bool isHorizontal ) noexcept;

    }

}

using namespace crimild;

SharedPointer< FrameGraphOperation > crimild::framegraph::gaussianBlur( SharedPointer< FrameGraphResource > const &image, Bool isHorizontal ) noexcept
{
    // TODO: move this to a compute pass
    auto renderPass = crimild::alloc< RenderPass >();
    renderPass->setName( std::string( "gaussianBlur" ) + ( isHorizontal ? "Horizontal" : "Vertical" ) );

    auto color = useColorAttachment( renderPass->getName() + "/color", useFormat( image ) );

    renderPass->attachments = { color };

    auto pipeline = [ & ] {
        auto pipeline = crimild::alloc< GraphicsPipeline >();
        pipeline->setProgram(
            [ & ] {
                auto program = crimild::alloc< ShaderProgram >(
                    Array< SharedPointer< Shader > > {
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
                                isHorizontal
                                    ? CRIMILD_TO_STRING(
                                        layout( location = 0 ) in vec2 inTexCoord;

                                        layout( set = 0, binding = 0 ) uniform sampler2D uColorMap;

                                        layout( location = 0 ) out vec4 outColor;

                                        void main() {
                                            const float weights[ 5 ] = float[]( 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 );
                                            vec2 offset = 1.0 / textureSize( uColorMap, 0 );
                                            vec3 result = texture( uColorMap, inTexCoord ).rgb;
                                            for ( int i = 0; i < 5; ++i ) {
                                                result += weights[ i ] * texture( uColorMap, inTexCoord + vec2( offset.x * i, 0.0 ) ).rgb;
                                                result += weights[ i ] * texture( uColorMap, inTexCoord - vec2( offset.x * i, 0.0 ) ).rgb;
                                            }
                                            outColor = vec4( result, 1.0 );
                                        } )
                                    : CRIMILD_TO_STRING(
                                        layout( location = 0 ) in vec2 inTexCoord;

                                        layout( set = 0, binding = 0 ) uniform sampler2D uColorMap;

                                        layout( location = 0 ) out vec4 outColor;

                                        void main() {
                                            const float weights[ 5 ] = float[]( 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 );
                                            vec2 offset = 1.0 / textureSize( uColorMap, 0 );
                                            vec3 result = texture( uColorMap, inTexCoord ).rgb;
                                            for ( int i = 0; i < 5; ++i ) {
                                                result += weights[ i ] * texture( uColorMap, inTexCoord + vec2( 0.0, offset.y * i ) ).rgb;
                                                result += weights[ i ] * texture( uColorMap, inTexCoord - vec2( 0.0, offset.y * i ) ).rgb;
                                            }
                                            outColor = vec4( result, 1.0 );
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
        return pipeline;
    }();

    auto descriptors = [ & ] {
        auto descriptorSet = crimild::alloc< DescriptorSet >();
        descriptorSet->descriptors = {
            Descriptor {
                .descriptorType = DescriptorType::TEXTURE,
                .obj = withResource( crimild::alloc< Texture >(), image ),
            },
        };
        return descriptorSet;
    }();

    // Use half-screen resolution
    renderPass->extent = {
        .scalingMode = ScalingMode::SWAPCHAIN_RELATIVE,
        .width = 0.5f,
        .height = 0.5f,
    };

    renderPass->reads( { image } );
    renderPass->writes( { color } );
    renderPass->produces( { color } );

    auto viewport = ViewportDimensions {
        .scalingMode = ScalingMode::RELATIVE,
    };

    return withGraphicsCommands(
        renderPass,
        [ pipeline, descriptors, viewport ]( auto commandBuffer ) {
            commandBuffer->setViewport( viewport );
            commandBuffer->setScissor( viewport );
            commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
            commandBuffer->bindDescriptorSet( crimild::get_ptr( descriptors ) );
            commandBuffer->draw( 6 );
        } );
}

SharedPointer< FrameGraphOperation > crimild::framegraph::gaussianBlur( SharedPointer< FrameGraphResource > const &image ) noexcept
{
    return gaussianBlur(
        useResource( gaussianBlur( image, true ) ),
        false );
}
