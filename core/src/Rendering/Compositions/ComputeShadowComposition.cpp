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

#include "Rendering/Compositions/ComputeShadowComposition.hpp"
#include "Rendering/CommandBuffer.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/ShadowMap.hpp"
#include "Rendering/Uniforms/CallbackUniformBuffer.hpp"
#include "SceneGraph/Geometry.hpp"
#include "Visitors/ApplyToGeometries.hpp"
#include "Visitors/FetchLights.hpp"

using namespace crimild;
using namespace crimild::compositions;

Composition crimild::compositions::computeShadow( SharedPointer< Node > const &scene ) noexcept
{
    return computeShadow( Composition { }, crimild::get_ptr( scene ) );
}

Composition crimild::compositions::computeShadow( Composition cmp, Node *scene ) noexcept
{
    FetchLights fetch;
    scene->perform( fetch );
    auto light = fetch.anyLight();
    if ( light == nullptr || !light->castShadows() ) {
        CRIMILD_LOG_WARNING( "ComputeShadow composition called with no lights" );
        return cmp;
    }

    auto pipeline = cmp.create< Pipeline >();
    pipeline->program = [&] {
        auto program = crimild::alloc< ShaderProgram >(
            Array< SharedPointer< Shader >> {
                crimild::alloc< Shader >(
                    Shader::Stage::VERTEX,
                    CRIMILD_TO_STRING(
                        layout ( location = 0 ) in vec3 inPosition;
                        layout ( location = 1 ) in vec3 inNormal;
                        layout ( location = 2 ) in vec2 inTextureCoord;

                        layout ( set = 0, binding = 0 ) uniform LightUniforms {
                            mat4 lightSpaceMatrix;
                        };

                        layout ( set = 1, binding = 0 ) uniform GeometryUniforms {
                            mat4 model;
                        };

                        void main()
                        {
                            gl_Position = lightSpaceMatrix * model * vec4( inPosition, 1.0 );
                        }
                    )
                ),
                crimild::alloc< Shader >(
                    Shader::Stage::FRAGMENT,
                    CRIMILD_TO_STRING(
                        layout ( location = 0 ) out vec4 fragColor;

                        float linearizeDepth(float depth, float nearPlane, float farPlane)
                        {
                            float z = depth * 2.0 - 1.0; // Back to NDC
                            return ( 2.0 * nearPlane * farPlane ) / ( farPlane + nearPlane - z * ( farPlane - nearPlane ) ) / farPlane;
                        }

                        void main()
                        {
                            float d = gl_FragCoord.z;
                            d = linearizeDepth( d, 1.0, 100.0 );
                            fragColor = vec4( vec3( d ), 1.0 );
                        }
                    )
                ),
            }
        );
        program->vertexLayouts = { VertexLayout::P3_N3_TC2 };
        program->descriptorSetLayouts = {
            [] {
                auto layout = crimild::alloc< DescriptorSetLayout >();
                layout->bindings = {
                    {
                        .descriptorType = DescriptorType::UNIFORM_BUFFER,
                        .stage = Shader::Stage::VERTEX,
                    },
                };
                return layout;
            }(),
            [] {
                auto layout = crimild::alloc< DescriptorSetLayout >();
                layout->bindings = {
                    {
                        .descriptorType = DescriptorType::UNIFORM_BUFFER,
                        .stage = Shader::Stage::VERTEX,
                    },
                };
                return layout;
            }(),
        };
//        pipeline->rasterizationState = RasterizationState {
        //          .cullMode = CullMode::FRONT,
        //};
        /*
          pipeline->depthStencilState = DepthStencilState {
          .depthTestEnable = true,
          .depthWriteEnable = true,
          .enable = true,
          .setBiasEnabled = true,
          .setBiasConstantFactor = 1.5f,
          .setBiasSlopeFactor = 2.5f,
          };
        */
        return program;
    }();
    pipeline->viewport = { .scalingMode = ScalingMode::DYNAMIC };
    pipeline->scissor = { .scalingMode = ScalingMode::DYNAMIC };

    auto renderPass = cmp.create< RenderPass >();
    renderPass->attachments = {
        [&] {
            auto att = cmp.createAttachment( "shadowColor" );
            att->usage = Attachment::Usage::COLOR_ATTACHMENT;
            att->format = Format::R8G8B8A8_UNORM;
            att->imageView = crimild::alloc< ImageView >();
            att->imageView->image = crimild::alloc< Image >();
            return crimild::retain( att );
        }(),
        [&] {
            auto att = cmp.createAttachment( "shadowDepth" );
            att->format = Format::DEPTH_STENCIL_DEVICE_OPTIMAL;
            att->imageView = crimild::alloc< ImageView >();
            att->imageView->image = crimild::alloc< Image >();
            return crimild::retain( att );
        }()
    };
    renderPass->setDescriptors(
        [&] {
            auto descriptors = crimild::alloc< DescriptorSet >();
            descriptors->descriptors = {
                {
                    .descriptorType = DescriptorType::UNIFORM_BUFFER,
                    .obj = [&] {
                        return crimild::alloc< CallbackUniformBuffer< Matrix4f >>(
                            [ light ] {
                                auto shadowMap = light->getShadowMap();
                                shadowMap->setLightProjectionMatrix( light->computeLightSpaceMatrix() );
                                return shadowMap->getLightProjectionMatrix();
                            }
                        );
                    }(),
                },
            };
            return descriptors;
        }()
    );
    renderPass->extent = {
        .scalingMode = ScalingMode::FIXED,
        .width = 512.0f,
        .height = 512.0f,
    };
    renderPass->clearValue = {
        .color = RGBAColorf( 1.0f, 1.0f, 1.0f, 1.0f ),
    };
    renderPass->commands = [&] {
        auto commandBuffer = crimild::alloc< CommandBuffer >();
        auto viewport = ViewportDimensions { .scalingMode = ScalingMode::RELATIVE };
        commandBuffer->setViewport( viewport );
        commandBuffer->setScissor( viewport );
        scene->perform(
            ApplyToGeometries(
                [&]( Geometry *geometry ) {
                    commandBuffer->bindGraphicsPipeline( crimild::get_ptr( pipeline ) );
                    commandBuffer->bindDescriptorSet( renderPass->getDescriptors() );
                    commandBuffer->bindDescriptorSet( geometry->getDescriptors() );
                    commandBuffer->drawPrimitive( geometry->anyPrimitive() );
                }
            )
        );
        return commandBuffer;
    }();

    cmp.setOutput( crimild::get_ptr( renderPass->attachments[ 1 ] ) );

    return cmp;
}
