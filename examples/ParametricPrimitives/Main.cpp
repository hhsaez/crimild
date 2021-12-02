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
        auto materialBuilder = []( auto primitiveType ) {
            auto material = crimild::alloc< UnlitMaterial >();
            material->setGraphicsPipeline(
                [ primitiveType ] {
                    auto pipeline = crimild::alloc< GraphicsPipeline >();
                    pipeline->primitiveType = primitiveType;
                    pipeline->setProgram(
                        [ & ] {
                            auto program = crimild::alloc< UnlitShaderProgram >();
                            program->setShaders(
                                Array< SharedPointer< Shader > > {
                                    Shader::withSource(
                                        Shader::Stage::VERTEX,
                                        { .path = "assets/shaders/scene.vert" } ),
                                    Shader::withSource(
                                        Shader::Stage::FRAGMENT,
                                        { .path = "assets/shaders/scene.frag" } ),
                                } );
                            program->vertexLayouts = { VertexP3C3::getLayout() };
                            return program;
                        }() );
                    return pipeline;
                }() );
            return material;
        };

        auto solidMaterial = materialBuilder( Primitive::Type::TRIANGLES );
        auto linesMaterial = materialBuilder( Primitive::Type::LINES );

        setScene(
            [ & ] {
                auto scene = crimild::alloc< Group >();

                // Spheres
                scene->attachNode(
                    [ & ] {
                        auto spheres = crimild::alloc< Group >();

                        auto sphere = [ & ]( const Vector3f &position, const Vector2i &divisions ) {
                            auto group = crimild::alloc< Group >();
                            group->attachNode(
                                [ & ] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive(
                                        crimild::alloc< ParametricSpherePrimitive >(
                                            ParametricSpherePrimitive::Params {
                                                .type = Primitive::Type::TRIANGLES,
                                                .layout = VertexP3C3::getLayout(),
                                                .radius = 1.0f,
                                                .colorMode = { .type = ParametricPrimitive::ColorMode::Type::POSITIONS },
                                                .divisions = divisions,
                                            } ) );
                                    geometry->attachComponent< MaterialComponent >( solidMaterial );
                                    return geometry;
                                }() );
                            group->attachNode(
                                [ & ] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive(
                                        crimild::alloc< ParametricSpherePrimitive >(
                                            ParametricSpherePrimitive::Params {
                                                .type = Primitive::Type::LINES,
                                                .layout = VertexP3C3::getLayout(),
                                                .radius = 1.1f,
                                                .divisions = divisions,
                                            } ) );
                                    geometry->attachComponent< MaterialComponent >( linesMaterial );
                                    return geometry;
                                }() );
                            group->setLocal( translation( position ) );
                            return group;
                        };

                        spheres->attachNode( sphere( Vector3 { -6.0f, 0.0f, 0.0f }, Vector2i { 40, 40 } ) );
                        spheres->attachNode( sphere( Vector3 { -3.0f, 0.0f, 0.0f }, Vector2i { 30, 30 } ) );
                        spheres->attachNode( sphere( Vector3 { 0.0f, 0.0f, 0.0f }, Vector2i { 20, 20 } ) );
                        spheres->attachNode( sphere( Vector3 { 3.0f, 0.0f, 0.0f }, Vector2i { 10, 10 } ) );
                        spheres->attachNode( sphere( Vector3 { 6.0f, 0.0f, 0.0f }, Vector2i { 5, 5 } ) );

                        return spheres;
                    }() );

                // Cones
                scene->attachNode(
                    [ & ] {
                        auto cones = crimild::alloc< Group >();

                        auto cone = [ & ]( const Vector3f &position, const Vector2i &divisions ) {
                            auto group = crimild::alloc< Group >();
                            group->attachNode(
                                [ & ] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive(
                                        crimild::alloc< ConePrimitive >(
                                            ConePrimitive::Params {
                                                .type = Primitive::Type::TRIANGLES,
                                                .layout = VertexP3C3::getLayout(),
                                                .colorMode = { .type = ParametricPrimitive::ColorMode::Type::POSITIONS },
                                                .divisions = divisions,
                                            } ) );
                                    geometry->attachComponent< MaterialComponent >( solidMaterial );
                                    return geometry;
                                }() );
                            group->attachNode(
                                [ & ] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive(
                                        crimild::alloc< ConePrimitive >(
                                            ConePrimitive::Params {
                                                .type = Primitive::Type::LINES,
                                                .layout = VertexP3C3::getLayout(),
                                                .radius = 0.6f,
                                                .height = 1.1f,
                                                .divisions = divisions,
                                            } ) );
                                    geometry->attachComponent< MaterialComponent >( linesMaterial );
                                    return geometry;
                                }() );
                            group->setLocal( translation( position ) );
                            return group;
                        };

                        cones->attachNode( cone( Vector3 { -6.0f, 0.0f, 0.0f }, Vector2i { 40, 40 } ) );
                        cones->attachNode( cone( Vector3 { -3.0f, 0.0f, 0.0f }, Vector2i { 30, 30 } ) );
                        cones->attachNode( cone( Vector3 { 0.0f, 0.0f, 0.0f }, Vector2i { 20, 20 } ) );
                        cones->attachNode( cone( Vector3 { 3.0f, 0.0f, 0.0f }, Vector2i { 10, 10 } ) );
                        cones->attachNode( cone( Vector3 { 6.0f, 0.0f, 0.0f }, Vector2i { 5, 5 } ) );

                        cones->setLocal( translation( 0.0f, 3.0f, 0.0f ) );

                        return cones;
                    }() );

                // KleinBottles
                scene->attachNode(
                    [ & ] {
                        auto kleinBottles = crimild::alloc< Group >();

                        auto kleinBottle = [ & ]( const Vector3f &position, const Vector2i &divisions ) {
                            auto group = crimild::alloc< Group >();
                            group->attachNode(
                                [ & ] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive(
                                        crimild::alloc< KleinBottlePrimitive >(
                                            KleinBottlePrimitive::Params {
                                                .type = Primitive::Type::TRIANGLES,
                                                .layout = VertexP3C3::getLayout(),
                                                .colorMode = { .type = ParametricPrimitive::ColorMode::Type::POSITIONS },
                                                .divisions = divisions,
                                            } ) );
                                    geometry->attachComponent< MaterialComponent >( solidMaterial );
                                    return geometry;
                                }() );
                            group->attachNode(
                                [ & ] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive(
                                        crimild::alloc< KleinBottlePrimitive >(
                                            KleinBottlePrimitive::Params {
                                                .type = Primitive::Type::LINES,
                                                .layout = VertexP3C3::getLayout(),
                                                .scale = 1.1f,
                                                .divisions = divisions,
                                            } ) );
                                    geometry->attachComponent< MaterialComponent >( linesMaterial );
                                    return geometry;
                                }() );
                            group->setLocal( translation( position ) );
                            return group;
                        };

                        kleinBottles->attachNode( kleinBottle( Vector3 { -6.0f, 0.0f, 0.0f }, Vector2i { 40, 40 } ) );
                        kleinBottles->attachNode( kleinBottle( Vector3 { -3.0f, 0.0f, 0.0f }, Vector2i { 30, 30 } ) );
                        kleinBottles->attachNode( kleinBottle( Vector3 { 0.0f, 0.0f, 0.0f }, Vector2i { 20, 20 } ) );
                        kleinBottles->attachNode( kleinBottle( Vector3 { 3.0f, 0.0f, 0.0f }, Vector2i { 10, 10 } ) );
                        kleinBottles->attachNode( kleinBottle( Vector3 { 6.0f, 0.0f, 0.0f }, Vector2i { 5, 5 } ) );

                        kleinBottles->setLocal( translation( 0.0f, -3.0f, 0.0f ) );

                        return kleinBottles;
                    }() );

                // Treefoil Knot
                scene->attachNode(
                    [ & ] {
                        auto trefoilKnots = crimild::alloc< Group >();

                        auto trefoilKnot = [ & ]( const Vector3f &position, const Vector2i &divisions ) {
                            auto group = crimild::alloc< Group >();
                            group->attachNode(
                                [ & ] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive(
                                        crimild::alloc< TrefoilKnotPrimitive >(
                                            TrefoilKnotPrimitive::Params {
                                                .type = Primitive::Type::TRIANGLES,
                                                .layout = VertexP3C3::getLayout(),
                                                .colorMode = { .type = ParametricPrimitive::ColorMode::Type::POSITIONS },
                                                .divisions = divisions,
                                            } ) );
                                    geometry->attachComponent< MaterialComponent >( solidMaterial );
                                    return geometry;
                                }() );
                            group->attachNode(
                                [ & ] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive(
                                        crimild::alloc< TrefoilKnotPrimitive >(
                                            TrefoilKnotPrimitive::Params {
                                                .type = Primitive::Type::LINES,
                                                .layout = VertexP3C3::getLayout(),
                                                .scale = 1.1f,
                                                .divisions = divisions,
                                            } ) );
                                    geometry->attachComponent< MaterialComponent >( linesMaterial );
                                    return geometry;
                                }() );
                            group->setLocal( translation( position ) );
                            return group;
                        };

                        trefoilKnots->attachNode( trefoilKnot( Vector3 { -6.0f, 0.0f, 0.0f }, Vector2i { 40, 40 } ) );
                        trefoilKnots->attachNode( trefoilKnot( Vector3 { -3.0f, 0.0f, 0.0f }, Vector2i { 30, 30 } ) );
                        trefoilKnots->attachNode( trefoilKnot( Vector3 { 0.0f, 0.0f, 0.0f }, Vector2i { 20, 20 } ) );
                        trefoilKnots->attachNode( trefoilKnot( Vector3 { 3.0f, 0.0f, 0.0f }, Vector2i { 10, 10 } ) );
                        trefoilKnots->attachNode( trefoilKnot( Vector3 { 6.0f, 0.0f, 0.0f }, Vector2i { 5, 5 } ) );

                        trefoilKnots->setLocal( translation( 0.0f, -6.0f, 0.0f ) );

                        return trefoilKnots;
                    }() );

                // MobiusStrips
                scene->attachNode(
                    [ & ] {
                        auto mobiusStrips = crimild::alloc< Group >();

                        auto mobiusStrip = [ & ]( const Vector3f &position, const Vector2i &divisions ) {
                            auto group = crimild::alloc< Group >();
                            group->attachNode(
                                [ & ] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive(
                                        crimild::alloc< MobiusStripPrimitive >(
                                            MobiusStripPrimitive::Params {
                                                .type = Primitive::Type::TRIANGLES,
                                                .layout = VertexP3C3::getLayout(),
                                                .colorMode = { .type = ParametricPrimitive::ColorMode::Type::POSITIONS },
                                                .divisions = divisions,
                                            } ) );
                                    geometry->attachComponent< MaterialComponent >( solidMaterial );
                                    return geometry;
                                }() );
                            group->attachNode(
                                [ & ] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive(
                                        crimild::alloc< MobiusStripPrimitive >(
                                            MobiusStripPrimitive::Params {
                                                .type = Primitive::Type::LINES,
                                                .layout = VertexP3C3::getLayout(),
                                                .scale = 1.1f,
                                                .divisions = divisions,
                                            } ) );
                                    geometry->attachComponent< MaterialComponent >( linesMaterial );
                                    return geometry;
                                }() );
                            group->setLocal( translation( position ) );
                            return group;
                        };

                        mobiusStrips->attachNode( mobiusStrip( Vector3 { -6.0f, 0.0f, 0.0f }, Vector2i { 40, 40 } ) );
                        mobiusStrips->attachNode( mobiusStrip( Vector3 { -3.0f, 0.0f, 0.0f }, Vector2i { 30, 30 } ) );
                        mobiusStrips->attachNode( mobiusStrip( Vector3 { 0.0f, 0.0f, 0.0f }, Vector2i { 20, 20 } ) );
                        mobiusStrips->attachNode( mobiusStrip( Vector3 { 3.0f, 0.0f, 0.0f }, Vector2i { 10, 10 } ) );
                        mobiusStrips->attachNode( mobiusStrip( Vector3 { 6.0f, 0.0f, 0.0f }, Vector2i { 5, 5 } ) );

                        mobiusStrips->setLocal( translation( 0.0f, -9.0f, 0.0f ) );

                        return mobiusStrips;
                    }() );

                // Toruss
                scene->attachNode(
                    [ & ] {
                        auto toruses = crimild::alloc< Group >();

                        auto torus = [ & ]( const Vector3f &position, const Vector2i &divisions ) {
                            auto group = crimild::alloc< Group >();
                            group->attachNode(
                                [ & ] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive(
                                        crimild::alloc< TorusPrimitive >(
                                            TorusPrimitive::Params {
                                                .type = Primitive::Type::TRIANGLES,
                                                .layout = VertexP3C3::getLayout(),
                                                .colorMode = { .type = ParametricPrimitive::ColorMode::Type::POSITIONS },
                                                .divisions = divisions,
                                            } ) );
                                    geometry->attachComponent< MaterialComponent >( solidMaterial );
                                    return geometry;
                                }() );
                            group->attachNode(
                                [ & ] {
                                    auto geometry = crimild::alloc< Geometry >();
                                    geometry->attachPrimitive(
                                        crimild::alloc< TorusPrimitive >(
                                            TorusPrimitive::Params {
                                                .type = Primitive::Type::LINES,
                                                .layout = VertexP3C3::getLayout(),
                                                .minorRadius = 0.2f,
                                                .majorRadius = 0.6f,
                                                .divisions = divisions,
                                            } ) );
                                    geometry->attachComponent< MaterialComponent >( linesMaterial );
                                    return geometry;
                                }() );
                            group->setLocal( translation( position ) );
                            return group;
                        };

                        toruses->attachNode( torus( Vector3 { -6.0f, 0.0f, 0.0f }, Vector2i { 40, 40 } ) );
                        toruses->attachNode( torus( Vector3 { -3.0f, 0.0f, 0.0f }, Vector2i { 30, 30 } ) );
                        toruses->attachNode( torus( Vector3 { 0.0f, 0.0f, 0.0f }, Vector2i { 20, 20 } ) );
                        toruses->attachNode( torus( Vector3 { 3.0f, 0.0f, 0.0f }, Vector2i { 10, 10 } ) );
                        toruses->attachNode( torus( Vector3 { 6.0f, 0.0f, 0.0f }, Vector2i { 5, 5 } ) );

                        toruses->setLocal( translation( 0.0f, -12.0f, 0.0f ) );

                        return toruses;
                    }() );

                scene->attachNode( [] {
                    auto camera = crimild::alloc< Camera >();
                    camera->setLocal( translation( 0.0f, -4.5f, 24.0f ) );
                    return camera;
                }() );
                return scene;
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Parametric Primitives" );
