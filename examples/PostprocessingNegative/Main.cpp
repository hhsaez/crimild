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
        auto rnd = Random::Generator( 1982 );

        setScene(
            [ & ] {
                auto scene = crimild::alloc< Group >();

                auto primitive = crimild::alloc< BoxPrimitive >(
                    BoxPrimitive::Params {
                        .type = Primitive::Type::TRIANGLES,
                        .layout = VertexP3N3TC2::getLayout(),
                    } );

                auto material = [] {
                    auto material = crimild::alloc< UnlitMaterial >();
                    material->setTexture(
                        [] {
                            auto texture = crimild::alloc< Texture >();
                            texture->imageView = [] {
                                auto imageView = crimild::alloc< ImageView >();
                                imageView->image = Image::CHECKERBOARD_4;
                                return imageView;
                            }();
                            texture->sampler = [] {
                                auto sampler = crimild::alloc< Sampler >();
                                sampler->setMinFilter( Sampler::Filter::NEAREST );
                                sampler->setMagFilter( Sampler::Filter::NEAREST );
                                return sampler;
                            }();
                            return texture;
                        }() );
                    return material;
                }();

                for ( auto i = 0; i < 30; ++i ) {
                    scene->attachNode(
                        [ & ] {
                            auto geometry = crimild::alloc< Geometry >();
                            geometry->attachPrimitive( primitive );

                            geometry->local().setTranslate(
                                rnd.generate( -10.0f, 10.0f ),
                                rnd.generate( -10.0f, 10.0f ),
                                rnd.generate( -10.0f, 10.0f ) );

                            geometry->local().setScale( rnd.generate( 0.75f, 1.5f ) );

                            geometry->local().rotate().fromAxisAngle(
                                Vector3f(
                                    rnd.generate( 0.01f, 1.0f ),
                                    rnd.generate( 0.01f, 1.0f ),
                                    rnd.generate( 0.01f, 1.0f ) )
                                    .getNormalized(),
                                rnd.generate( 0.0f, Numericf::TWO_PI ) );

                            geometry->attachComponent< MaterialComponent >()->attachMaterial( material );

                            return geometry;
                        }() );
                }

                scene->attachNode(
                    [ & ] {
                        auto camera = crimild::alloc< Camera >();
                        camera->local().setTranslate( 0.0f, 0.0f, 30.0f );
                        return camera;
                    }() );

                scene->perform( StartComponents() );

                return scene;
            }() );

        RenderSystem::getInstance()->setFrameGraph(
            framegraph::invert(
                framegraph::useResource(
                    RenderSystem::getInstance()->getFrameGraph() ) ) );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Postprocessing: Negative" );
