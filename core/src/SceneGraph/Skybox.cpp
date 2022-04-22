/*
 * Copyright (c) 2002-present, H. Hernan Saez
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

#include "SceneGraph/Skybox.hpp"

#include "Components/MaterialComponent.hpp"
#include "Mathematics/swizzle.hpp"
#include "Primitives/BoxPrimitive.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Materials/SkyboxMaterial.hpp"
#include "Rendering/Materials/UnlitMaterial.hpp"
#include "Rendering/Pipeline.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/UniformBuffer.hpp"

using namespace crimild;

Skybox::Skybox( const ColorRGB &color ) noexcept
{
    setLayer( Node::Layer::SKYBOX );

    attachPrimitive(
        crimild::alloc< BoxPrimitive >(
            BoxPrimitive::Params {
                .type = Primitive::Type::TRIANGLES,
                .layout = VertexP3N3TC2::getLayout(),
                .size = Vector3f { 10.0f, 10.0f, 10.0f },
                .invertFaces = true,
            } ) );

    attachComponent< MaterialComponent >(
        [ color ] {
            auto material = crimild::alloc< UnlitMaterial >();
            material->setProgram(
                crimild::alloc< ShaderProgram >(
                    Array< SharedPointer< Shader > > {
                        crimild::alloc< Shader >(
                            Shader::Stage::FRAGMENT,
                            R"(
                                void frag_main( inout Fragment frag )
                                {
                                    vec3 skyColor = frag.color;
                                    vec3 groundColor = vec3( 0.1, 0.1, 0.0 );
                                    vec3 horizonColor = vec3( 1.0, 1.0, 1.0 );
                                    float e = 0.75;
                                    float y = normalize( frag.worldPosition ).y;

                                    frag.color = mix(horizonColor, y < 0 ? groundColor : skyColor, pow( abs( y ), e ) );
                                }
                            )" ),
                    } ) );
            material->setColor( rgba( color ) );
            return material;
        }() );
}

Skybox::Skybox( SharedPointer< Texture > const &texture ) noexcept
{
    setLayer( Node::Layer::SKYBOX );

    attachPrimitive(
        crimild::alloc< BoxPrimitive >(
            BoxPrimitive::Params {
                .type = Primitive::Type::TRIANGLES,
                .layout = VertexP3::getLayout(),
                .size = Vector3f { 10.0f, 10.0f, 10.0f },
                .invertFaces = true,
            } ) );

    attachComponent< MaterialComponent >()->attachMaterial(
        [ & ]() -> SharedPointer< Material > {
            if ( texture->imageView->image->type == Image::Type::IMAGE_2D_CUBEMAP ) {
                auto material = crimild::alloc< SkyboxMaterial >();
                material->setTexture( texture );
                return material;
            }

            auto material = crimild::alloc< Material >();
            material->setGraphicsPipeline(
                [] {
                    auto pipeline = crimild::alloc< GraphicsPipeline >();
                    pipeline->primitiveType = Primitive::Type::TRIANGLES;
                    pipeline->setProgram(
                        [ & ] {
                            auto program = crimild::alloc< ShaderProgram >(
                                Array< SharedPointer< Shader > > {
                                    crimild::alloc< Shader >(
                                        Shader::Stage::VERTEX,
                                        R"(
                                        layout ( location = 0 ) in vec3 inPosition;

                                        layout ( set = 0, binding = 0 ) uniform RenderPassUniforms {
                                            mat4 view;
                                            mat4 proj;
                                        };

                                        layout ( set = 2, binding = 0 ) uniform GeometryUniforms {
                                            mat4 model;
                                        };

                                        layout ( location = 0 ) out vec3 outPosition;

                                        void main()
                                        {
                                            gl_Position = proj * mat4( mat3 ( view ) ) * model * vec4( inPosition, 1.0 );
                                            gl_Position = gl_Position.xyww;
                                            outPosition = inPosition;
                                        }
                                    )" ),
                                    crimild::alloc< Shader >(
                                        Shader::Stage::FRAGMENT,
                                        R"(
                                        layout ( location = 0 ) in vec3 inPosition;

                                        layout ( set = 1, binding = 0 ) uniform sampler2D uHDRMap;

                                        layout ( location = 0 ) out vec4 outColor;

                                        const vec2 invAtan = vec2( 0.1591, 0.3183 );

                                        vec2 sampleSphericalMap( vec3 v )
                                        {
                                            vec2 uv = vec2( atan( v.z, v.x ), asin( v.y ) );
                                            uv *= invAtan;
                                            uv += 0.5;
                                            uv.y = 1.0 - uv.y; // because of vulkan
                                            return uv;
                                        }

                                        void main() {
                                            vec2 uv = sampleSphericalMap( normalize( inPosition ) );
                                            vec3 color = texture( uHDRMap, uv ).rgb;
                                            outColor = vec4( color, 1.0 );
                                        }
                                    )" ) } );
                            program->vertexLayouts = { VertexP3::getLayout() };
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
                                            .descriptorType = DescriptorType::TEXTURE,
                                            .stage = Shader::Stage::FRAGMENT,
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
                            return program;
                        }() );
                    pipeline->viewport = { .scalingMode = ScalingMode::DYNAMIC };
                    pipeline->scissor = { .scalingMode = ScalingMode::DYNAMIC };
                    return pipeline;
                }() );
            material->setDescriptors(
                [ & ] {
                    auto descriptors = crimild::alloc< DescriptorSet >();
                    descriptors->descriptors = {
                        {
                            .descriptorType = DescriptorType::TEXTURE,
                            .obj = texture,
                        },
                    };
                    return descriptors;
                }() );
            return material;
        }() );
}

void Skybox::encode( coding::Encoder &encoder )
{
    Geometry::encode( encoder );
}

void Skybox::decode( coding::Decoder &decoder )
{
    Geometry::decode( decoder );
}
