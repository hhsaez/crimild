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

#include <Crimild.hpp>

using namespace crimild;

class Example : public Simulation {
public:
    virtual Event handle( const Event &e ) noexcept override
    {
        const auto ret = Simulation::handle( e );
        if ( ret.type == Event::Type::SIMULATION_START ) {
            // TODO: load scene
            onStarted();
        }
        return ret;
    }

    virtual void onStarted( void ) noexcept override
    {
        setScene(
            [ & ] {
                auto scene = crimild::alloc< Group >();

                scene->attachNode( [ & ] {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive(
                        [ & ] {
                            auto primitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
                            primitive->setVertexData(
                                {
                                    [ & ] {
                                        return crimild::alloc< VertexBuffer >(
                                            VertexP3N3TC2::getLayout(),
                                            Array< VertexP3N3TC2 > {
                                                {
                                                    .position = Vector3f { -0.5f, -0.5f, 0.0f },
                                                    // .color = ColorRGB { 1.0f, 0.0f, 0.0f },
                                                },
                                                {
                                                    .position = Vector3f { 0.5f, -0.5f, 0.0f },
                                                    // .color = ColorRGB { 0.0f, 1.0f, 0.0f },
                                                },
                                                {
                                                    .position = Vector3f { 0.0f, 0.5f, 0.0f },
                                                    // .color = ColorRGB { 0.0f, 0.0f, 1.0f },
                                                },
                                            } );
                                    }(),
                                } );
                            primitive->setIndices(
                                crimild::alloc< IndexBuffer >(
                                    Format::INDEX_32_UINT,
                                    Array< crimild::UInt32 > {
                                        0,
                                        1,
                                        2,
                                    } ) );
                            return primitive;
                        }() );
                    geometry->attachComponent< MaterialComponent >(
                        [] {
                            auto material = crimild::alloc< UnlitMaterial >();
                            material->setGraphicsPipeline(
                                [] {
                                    auto pipeline = crimild::alloc< GraphicsPipeline >();
                                    pipeline->setProgram(
                                        [] {
                                            auto program = crimild::alloc< UnlitShaderProgram >();
                                            program->setShaders(
                                                Array< SharedPointer< Shader > > {
                                                    crimild::alloc< Shader >(
                                                        Shader::Stage::VERTEX,
                                                        R"(
                                                            layout( location = 0 ) in vec3 inPosition;
                                                            layout( location = 1 ) in vec3 inColor;

                                                            layout( set = 0, binding = 0 ) uniform RenderPassUniforms
                                                            {
                                                                mat4 view;
                                                                mat4 proj;
                                                            };

                                                            layout( set = 2, binding = 0 ) uniform GeometryUniforms
                                                            {
                                                                mat4 model;
                                                            };

                                                            layout( location = 0 ) out vec3 outColor;

                                                            void main()
                                                            {
                                                                gl_Position = proj * view * model * vec4( inPosition, 1.0 );
                                                                outColor = inColor;
                                                            }
                                                        )" ),
                                                    crimild::alloc< Shader >(
                                                        Shader::Stage::FRAGMENT,
                                                        R"(
                                                            layout( location = 0 ) in vec3 inColor;

                                                            layout( location = 0 ) out vec4 outColor;

                                                            void main()
                                                            {
                                                                outColor = vec4( inColor, 1.0 );
                                                            }
                                                        )" ),
                                                } );
                                            program->vertexLayouts = { VertexLayout::P3_C3 };
                                            return program;
                                        }() );
                                    pipeline->rasterizationState.cullMode = CullMode::NONE;
                                    return pipeline;
                                }() );
                            return material;
                        }() );
                    return geometry;
                }() );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >();
                    camera->setLocal(
                        lookAt(
                            Point3 { 0, 0, 3 },
                            Point3 { 0, 0, 0 },
                            Vector3::Constants::UP ) );
                    return camera;
                }() );
                return scene;
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Triangle" );
