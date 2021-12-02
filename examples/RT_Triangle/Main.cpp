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
    void onStarted( void ) noexcept override
    {
        setScene(
            [ & ] {
                auto scene = crimild::alloc< Group >();

                scene->attachNode(
                    [] {
                        auto geometry = crimild::alloc< Geometry >();

#if 1
                        geometry->attachPrimitive(
                            [] {
                                auto primitive = crimild::alloc< Primitive >( Primitive::Type::TRIANGLES );
                                primitive->setVertexData(
                                    {
                                        [ & ] {
                                            return crimild::alloc< VertexBuffer >(
                                                VertexP3N3TC2::getLayout(),
                                                Array< VertexP3N3TC2 > {
                                                    {
                                                        .position = Vector3f { -1, 1, 0.0f },
                                                        .normal = Normal3 { 0, 0, 1 },

                                                    },
                                                    {
                                                        .position = Vector3f { -1, -1, 0.0f },
                                                        .normal = Normal3 { 0, 0, 1 },

                                                    },
                                                    {
                                                        .position = Vector3f { 1, -1, 0.0f },
                                                        .normal = Normal3 { 0, 0, 1 },

                                                    },
                                                    {
                                                        .position = Vector3f { 1, 1, 0.0f },
                                                        .normal = Normal3 { 0, 0, 1 },

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
                                            0,
                                            2,
                                            3,
                                        } ) );
                                return primitive;
                            }() );
#else
                        geometry->attachPrimitive( crimild::alloc< SpherePrimitive >() );
#endif

                        geometry->attachComponent< MaterialComponent >(
                            [] {
                                auto material = crimild::alloc< materials::PrincipledBSDF >();
                                material->setAlbedo( ColorRGB { 1, 0, 0 } );
                                return material;
                            }() );
                        return geometry;
                    }() );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >( 20, 4.0 / 3.0, 0.1f, 1000.0f );
                    camera->setLocal(
                        lookAt(
                            Point3 { 0, 0, 10 },
                            Point3 { 0, 0, 0 },
                            Vector3::Constants::UP ) );
                    camera->setFocusDistance( 10 );
                    camera->setAperture( 0.0f );
                    camera->attachComponent< FreeLookCameraComponent >();
                    return camera;
                }() );

                scene->attachNode( crimild::alloc< Skybox >( ColorRGB { 0.5f, 0.6f, 0.7f } ) );

                Simulation::getInstance()->getSettings()->set( "rt.background_color.r", 0.5f );
                Simulation::getInstance()->getSettings()->set( "rt.background_color.g", 0.6f );
                Simulation::getInstance()->getSettings()->set( "rt.background_color.b", 0.7f );

                scene->perform( UpdateWorldState() );
                scene->perform( StartComponents() );

                return scene;
            }() );

        // Use soft RT by default
        if ( Simulation::getInstance()->getSettings()->get< std::string >( "video.render_path", "default" ) == "default" ) {
            RenderSystem::getInstance()->useRTSoftRenderPath();
        }
    }
};

CRIMILD_CREATE_SIMULATION( Example, "RT: Triangle" );
