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

namespace crimild {

    class CustomParticleRenderer : public ParticleSystemComponent::ParticleRenderer {
        CRIMILD_IMPLEMENT_RTTI( CustomParticleRenderer )

    private:
        struct Vertex {
            Vector3f position;
        };

    public:
        CustomParticleRenderer( void ) noexcept
        {
        }

        ~CustomParticleRenderer( void ) = default;

        virtual void configure( Node *node, ParticleData *particles ) override
        {
            auto primitive = crimild::alloc< Primitive >( Primitive::Type::POINTS );

            auto vertexLayout = VertexLayout(
                Array< VertexAttribute > {
                    { VertexAttribute::Name::POSITION, utils::getFormat< Vector3f >() },
                    { VertexAttribute::Name::COLOR, utils::getFormat< RGBAColorf >() },
                    { VertexAttribute::Name::SIZE, utils::getFormat< Real32 >() },
                    { VertexAttribute::Name::TEX_COORD, utils::getFormat< Real32 >() },
                } );
            m_vertices = crimild::alloc< VertexBuffer >( vertexLayout, particles->getParticleCount() );
            m_vertices->getBufferView()->setUsage( BufferView::Usage::DYNAMIC );
            primitive->setVertexData( { m_vertices } );

            auto geometry = crimild::alloc< Geometry >();
            geometry->attachPrimitive( primitive );
            geometry->attachComponent< MaterialComponent >()->attachMaterial(
                [ vertexLayout ] {
                    auto material = crimild::alloc< Material >();
                    material->setGraphicsPipeline(
                        [ vertexLayout ] {
                            auto pipeline = crimild::alloc< GraphicsPipeline >();
                            pipeline->primitiveType = Primitive::Type::POINTS;
                            pipeline->setProgram(
                                [ & ] {
                                    auto program = crimild::alloc< ShaderProgram >(
                                        Array< SharedPointer< Shader > > {
                                            Shader::withSource(
                                                Shader::Stage::VERTEX,
                                                { .path = "assets/shaders/particle.vert" } ),
                                            Shader::withSource(
                                                Shader::Stage::FRAGMENT,
                                                { .path = "assets/shaders/particle.frag" } ),
                                        } );
                                    program->vertexLayouts = { vertexLayout };
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
                                            layout->bindings = {};
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
                            pipeline->colorBlendState = ColorBlendState {
                                .enable = true,
                            };
                            return pipeline;
                        }() );
                    material->setDescriptors(
                        [ & ] {
                            auto descriptors = crimild::alloc< DescriptorSet >();
                            return descriptors;
                        }() );
                    material->setCastShadows( false );
                    return material;
                }() );
            static_cast< Group * >( node )->attachNode( geometry );

            m_positions = particles->getAttrib( ParticleAttrib::POSITION );
            m_colors = particles->getAttrib( ParticleAttrib::COLOR );
            m_sizes = particles->getAttrib( ParticleAttrib::UNIFORM_SCALE );
        }

        virtual void update( Node *node, crimild::Real64 dt, ParticleData *particles ) override
        {
            const auto N = particles->getParticleCount();

            const auto pCount = particles->getAliveCount();
            if ( pCount == 0 ) {
                return;
            }

            const auto srcPositions = m_positions->getData< Vector3f >();
            const auto srcColors = m_colors->getData< RGBAColorf >();
            const auto srcSizes = m_sizes->getData< Real32 >();

            auto dstPositions = m_vertices->get( VertexAttribute::Name::POSITION );
            auto dstColors = m_vertices->get( VertexAttribute::Name::COLOR );
            auto dstSizes = m_vertices->get( VertexAttribute::Name::SIZE );
            auto dstAlive = m_vertices->get( VertexAttribute::Name::TEX_COORD );

            // TODO: We should only account for live particles. But render passes
            // are not optimized to generate command buffers on the fly yet.
            // That's why setting a value for buffer view's length has no effect
            //m_vertices->getBufferView()->setLength( pCount );

            for ( auto i = 0; i < N; i++ ) {
                dstPositions->set( i, srcPositions[ i ] );
                dstColors->set( i, srcColors[ i ] );
                dstSizes->set( i, srcSizes[ i ] );
                dstAlive->set( i, Real32( i < pCount ? 1.0 : 0.0 ) );
            }
        }

    private:
        SharedPointer< VertexBuffer > m_vertices;
        ParticleAttribArray *m_positions = nullptr;
        ParticleAttribArray *m_colors = nullptr;
        ParticleAttribArray *m_sizes = nullptr;
    };

}

#define USE_PBR 1

class Particles : public Simulation {
public:
    virtual void onStarted( void ) noexcept override
    {
        setScene(
            [ & ] {
                auto scene = crimild::alloc< Group >();

                scene->attachNode(
                    [] {
                        auto primitive = crimild::alloc< SpherePrimitive >(
                            SpherePrimitive::Params {
                                .type = Primitive::Type::TRIANGLES,
                                .layout = VertexP3N3TC2::getLayout(),
                            } );

                        auto geometry = crimild::alloc< Geometry >();
                        geometry->attachPrimitive( primitive );
                        geometry->local().setTranslate( 0.0f, -3.5f, 0.0f );
                        geometry->local().setScale( 4.0f );
                        geometry->attachComponent< MaterialComponent >()->attachMaterial(
                            [] {
#if USE_PBR
                                auto material = crimild::alloc< LitMaterial >();
                                material->setAlbedo( RGBColorf( 1.0f, 1.0f, 0.75f ) );
                                material->setMetallic( 0.0 );
                                material->setRoughness( 1.0 );
#else
                                auto material = crimild::alloc< SimpleLitMaterial >();
                                material->setDiffuse( RGBAColorf( 1.0f, 0.0f, 1.0f, 1.0f ) );
#endif
                                return material;
                            }() );
                        return geometry;
                    }() );

                scene->attachNode(
                    [] {
                        auto geometry = crimild::alloc< Geometry >();
                        geometry->attachPrimitive( crimild::alloc< QuadPrimitive >( QuadPrimitive::Params { .size = 30.0f * Vector2f::ONE } ) );
                        geometry->local().rotate().fromAxisAngle( Vector3f::UNIT_X, -Numericf::HALF_PI );
                        geometry->attachComponent< MaterialComponent >()->attachMaterial(
                            [] {
#if USE_PBR
                                auto material = crimild::alloc< LitMaterial >();
                                material->setAlbedo( RGBColorf( 0.0f, 0.2f, 0.0f ) );
                                material->setMetallic( 0.0f );
                                material->setRoughness( 1.0f );
#else
                                auto material = crimild::alloc< SimpleLitMaterial >();
                                material->setDiffuse( RGBAColorf( 0.0f, 0.2f, 0.0f, 1.0f ) );
#endif
                                return material;
                            }() );
                        return geometry;
                    }() );

                scene->attachNode(
                    [] {
                        auto group = crimild::alloc< Group >();

                        auto primitive = crimild::alloc< BoxPrimitive >(
                            BoxPrimitive::Params {
                                .layout = VertexP3N3TC2::getLayout(),
                                .size = Vector3f( 0.25f, 0.25f, 1.0f ) } );

#if USE_PBR
                        auto material = crimild::alloc< LitMaterial >();
                        material->setMetallic( 0.1f );
                        material->setRoughness( 0.78f );
                        material->setAlbedo( RGBColorf( 0.47f, 0.15f, 0.12f ) );
#else
                        auto material = crimild::alloc< SimpleLitMaterial >();
                        material->setDiffuse( RGBAColorf( 0.47f, 0.15f, 0.12f, 1.0f ) );
#endif
                        material->getGraphicsPipeline()->rasterizationState = RasterizationState {
                            .cullMode = CullMode::NONE,
                        };

                        auto createLog = [ primitive, material ]( auto angle ) {
                            auto geometry = crimild::alloc< Geometry >();
                            geometry->setLocal(
                                [ angle ] {
                                    Transformation t;
                                    t.rotate().fromAxisAngle( Vector3f::UNIT_Z, angle );
                                    return t;
                                }() );
                            geometry->attachPrimitive( primitive );
                            geometry->attachComponent< MaterialComponent >( material );
                            return geometry;
                        };

                        auto createLog2 = [ createLog ]( auto angle ) {
                            auto group = crimild::alloc< Group >();
                            auto log = crimild::alloc< Group >();
                            log->attachNode( createLog( 0.0f ) );
                            log->attachNode( createLog( 0.5f ) );
                            log->setLocal(
                                [] {
                                    Transformation t;
                                    t.setTranslate( 0.0f, 0.0f, 1.0f );
                                    t.rotate().fromAxisAngle( Vector3f::UNIT_X, Random::generate< float >( 0.075f, 0.15f ) * Numericf::PI );
                                    return t;
                                }() );
                            group->attachNode( log );
                            group->setLocal(
                                [ angle ] {
                                    Transformation t;
                                    t.rotate().fromAxisAngle( Vector3f::UNIT_Y, angle );
                                    return t;
                                }() );
                            return group;
                        };

                        for ( auto phi = 0.0f; phi < Numericf::TWO_PI; phi += 0.5f ) {
                            group->attachNode( createLog2( phi ) );
                        }

                        group->setLocal(
                            [] {
                                Transformation t;
                                t.setTranslate( 0.0f, 0.25f, 0.0f );
                                return t;
                            }() );

                        return group;
                    }() );

                scene->attachNode(
                    [] {
                        auto group = crimild::alloc< Group >();
                        group->attachNode(
                            [] {
                                auto geometry = crimild::alloc< Geometry >();
                                geometry->attachPrimitive(
                                    crimild::alloc< SpherePrimitive >(
                                        SpherePrimitive::Params {
                                            .type = Primitive::Type::TRIANGLES,
                                            .layout = VertexP3N3TC2::getLayout(),
                                            .radius = 0.1f,
                                        } ) );
                                geometry->attachComponent< MaterialComponent >()->attachMaterial(
                                    [] {
                                        auto material = crimild::alloc< UnlitMaterial >();
                                        material->setColor( RGBAColorf::ONE );
                                        material->setCastShadows( false );
                                        return material;
                                    }() );
                                geometry->setEnabled( Simulation::getInstance()->getSettings()->get< Bool >( "debug", false ) );
                                return geometry;
                            }() );
                        group->attachNode(
                            [] {
                                auto light = crimild::alloc< Light >( Light::Type::POINT );
#if USE_PBR
                                light->setColor( RGBAColorf( 20.0f, 13.0f, 1.0f, 1.0f ) );
#else
                                light->setColor( 0.5f * RGBAColorf::ONE );
#endif
                                light->setCastShadows( true );
                                light->getShadowMap()->setBias( 0.025f );
                                return light;
                            }() );
                        group->attachComponent< LambdaComponent >(
                            []( auto node, auto &clock ) {
                                const auto delta = 0.05f;
                                node->local().setTranslate(
                                    Random::generate< float >( -delta, delta ),
                                    1.5f + Random::generate< float >( -2.0f * delta, 2.0f * delta ),
                                    Random::generate< float >( -delta, delta ) );
                            } );
                        return group;
                    }() );

#if USE_PBR
                scene->attachNode( crimild::alloc< Skybox >( RGBColorf::ZERO ) );
#endif

                scene->attachNode(
                    [] {
                        auto fire = crimild::alloc< Group >();
                        auto ps = fire->attachComponent< ParticleSystemComponent >( 500 );
                        ps->setPreWarmTime( 1.0 );
                        ps->setEmitRate( 200 );
                        ps->addGenerator< BoxPositionParticleGenerator >( Vector3f::ZERO, Vector3f( 0.5f, 0.25f, 0.5f ) );
                        ps->addGenerator< RandomVector3fParticleGenerator >( ParticleAttrib::VELOCITY, Vector3f( 0.0f, 0.25f, 0.0f ), Vector3f( 0.2f, 1.0f, 0.2f ) );
                        ps->addGenerator(
                            [] {
                                auto generator = crimild::alloc< DefaultVector3fParticleGenerator >();
                                generator->setParticleAttribType( ParticleAttrib::ACCELERATION );
                                generator->setValue( Vector3f::ZERO );
                                return generator;
                            }() );
                        ps->addGenerator(
                            [] {
                                auto generator = crimild::alloc< ColorParticleGenerator >();
                                generator->setMinStartColor( RGBAColorf( 10.0f, 5.0f, 2.0f, 0.1f ) );
                                generator->setMaxStartColor( RGBAColorf( 5.0f, 5.0f, 1.0f, 0.1f ) );
                                generator->setMinEndColor( RGBAColorf( 0.0f, 0.0f, 0.0f, 0.1f ) );
                                generator->setMaxEndColor( RGBAColorf( 0.15f, 0.125f, 0.0f, 0.5f ) );
                                return generator;
                            }() );
                        ps->addGenerator(
                            [] {
                                auto generator = crimild::alloc< RandomReal32ParticleGenerator >();
                                generator->setParticleAttribType( ParticleAttrib::UNIFORM_SCALE );
                                generator->setMinValue( 0.05f );
                                generator->setMaxValue( 0.25f );
                                return generator;
                            }() );
                        ps->addGenerator(
                            [] {
                                auto generator = crimild::alloc< TimeParticleGenerator >();
                                generator->setMinTime( 2.0f );
                                generator->setMaxTime( 3.0f );
                                return generator;
                            }() );

                        ps->addUpdater( crimild::alloc< EulerParticleUpdater >() );
                        ps->addUpdater( crimild::alloc< ColorParticleUpdater >() );
                        ps->addUpdater( crimild::alloc< TimeParticleUpdater >() );
                        ps->addUpdater( crimild::alloc< CameraSortParticleUpdater >() );

                        ps->addRenderer( crimild::alloc< CustomParticleRenderer >() );

                        return fire;
                    }() );

                scene->attachNode(
                    [ & ] {
                        auto camera = crimild::alloc< Camera >();
                        camera->local().setTranslate( 0.0f, 5.0f, 5.0f );
                        camera->local().lookAt( Vector3f::UNIT_Y );
                        camera->attachComponent< FreeLookCameraComponent >();
                        return camera;
                    }() );

                scene->perform( StartComponents() );

                return scene;
            }() );
    }
};

CRIMILD_CREATE_SIMULATION( Particles, "Particles: Fire" );
