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
#include <Crimild_Import.hpp>

using namespace crimild;
using namespace crimild::import;

class MusicTrigger : public NodeComponent {
public:
    MusicTrigger( crimild::Real32 minDistance )
        : _minDistance( minDistance )
    {
    }

    virtual ~MusicTrigger( void )
    {
    }

    virtual void start( void ) override
    {
        auto source = getComponent< AudioSourceComponent >()->getAudioSource();
        source->setLoop( true );
        source->play();
    }

    virtual void update( const Clock &c ) override
    {
        auto camera = Camera::getMainCamera();

        auto source = getComponent< AudioSourceComponent >()->getAudioSource();

        auto d = Distance::compute( camera->getWorld().getTranslate(), getNode()->getWorld().getTranslate() );
        if ( d <= _minDistance ) {
            source->play();
        } else {
            source->pause();
        }
    }

private:
    crimild::Real32 _minDistance = 1.0f;
};

class BackgroundMusic : public NodeComponent {
    CRIMILD_IMPLEMENT_RTTI( crimild::examples::BackgroundMusic )
public:
    virtual void start( void ) override
    {
        auto source = getComponent< AudioSourceComponent >()->getAudioSource();
        if ( source != nullptr ) {
            source->setLoop( true );
            source->setVolume( 0.5f );
            source->play();
        }
    }
};

class Example : public Simulation {
public:
    virtual void onStarted( void ) noexcept override
    {
        setScene( [ & ] {
            auto scene = crimild::alloc< Group >();
            auto settings = Simulation::getInstance()->getSettings();

            scene->attachNode(
                [ & ] {
                    auto camera = crimild::alloc< Camera >();
                    camera->local().setTranslate( Vector3f( 0.0f, 3.0f, 3.0f ) );
                    camera->attachComponent< FreeLookCameraComponent >();
                    //camera->attachComponent< AudioListenerComponent >();
                    return camera;
                }() );

            scene->attachNode(
                [] {
                    auto path = FilePath {
                        .path = "assets/models/violin.fbx",
                    };

                    auto group = crimild::alloc< Group >();

                    SceneImporter importer;
                    auto model = importer.import( path.getAbsolutePath() );
                    if ( model != nullptr ) {
                        group->attachNode( model );
                    }
                    group->local().setTranslate( 1.0, 0.0, -1.0 );
                    group->local().rotate().fromEulerAngles( 0.0, 90.0, 0.0 );
                    group->attachComponent< AudioSourceComponent >(
                        AudioSystem::getInstance()->createAudioSource(
                            FileSystem::getInstance().pathForResource( "assets/music/violin.wav" ),
                            false ) );
                    group->attachComponent< MusicTrigger >( 2.0f );
                    return group;
                }() );

            scene->attachNode(
                [] {
                    auto path = FilePath {
                        .path = "assets/models/cello.fbx",
                    };

                    auto group = crimild::alloc< Group >();

                    SceneImporter importer;
                    auto model = importer.import( path.getAbsolutePath() );
                    if ( model != nullptr ) {
                        group->attachNode( model );
                    }
                    group->local().setTranslate( 4.0, 0.0, -4.0 );
                    group->local().rotate().fromEulerAngles( 0.0, 0.0, 0.0 );
                    group->attachComponent< AudioSourceComponent >(
                        AudioSystem::getInstance()->createAudioSource(
                            FileSystem::getInstance().pathForResource( "assets/music/chello.wav" ),
                            false ) );
                    group->attachComponent< MusicTrigger >( 3.0f );
                    return group;
                }() );

            scene->attachNode(
                [] {
                    auto path = FilePath {
                        .path = "assets/models/piano.obj",
                    };

                    auto group = crimild::alloc< Group >();

                    SceneImporter importer;
                    auto model = importer.import( path.getAbsolutePath() );
                    if ( model != nullptr ) {
                        group->attachNode( model );
                    }
                    group->local().setTranslate( -3.18606, 0.0, -3.31557 );
                    group->local().rotate().fromEulerAngles( 0.0, 45.0, 0.0 );
                    group->attachComponent< AudioSourceComponent >(
                        AudioSystem::getInstance()->createAudioSource(
                            FileSystem::getInstance().pathForResource( "assets/music/piano.wav" ),
                            false ) );
                    group->attachComponent< MusicTrigger >( 5.0f );
                    return group;
                }() );

            scene->attachNode(
                [ & ] {
                    if ( !settings->hasKey( "skybox" ) ) {
                        return crimild::alloc< Skybox >( RGBColorf( 0.025f, 0.036f, 0.09f ) );
                    }
                    return crimild::alloc< Skybox >(
                        [ settings = getSettings() ] {
                            auto texture = crimild::alloc< Texture >();
                            texture->imageView = [ settings ] {
                                auto imageView = crimild::alloc< ImageView >();
                                imageView->image = ImageManager::getInstance()->loadImage(
                                    {
                                        .filePath = {
                                            .path = settings->get< std::string >( "skybox" ),
                                            .pathType = FilePath::PathType::ABSOLUTE,
                                        },
                                        .hdr = true,
                                    } );
                                return imageView;
                            }();
                            texture->sampler = [ & ] {
                                auto sampler = crimild::alloc< Sampler >();
                                sampler->setMinFilter( Sampler::Filter::LINEAR );
                                sampler->setMagFilter( Sampler::Filter::LINEAR );
                                sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_BORDER );
                                sampler->setCompareOp( CompareOp::NEVER );
                                return sampler;
                            }();
                            return texture;
                        }() );
                }() );

            scene->attachNode(
                [] {
                    auto light = crimild::alloc< Light >( Light::Type::DIRECTIONAL );
                    light->setColor( RGBAColorf::ONE );
                    light->setEnergy( 10.0f );
                    light->local().setTranslate( Vector3f::ONE );
                    light->local().lookAt( Vector3f::ZERO );
                    light->setCastShadows( true );
                    return light;
                }() );

            scene->attachNode(
                [] {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive(
                        crimild::alloc< QuadPrimitive >(
                            QuadPrimitive::Params {} ) );
                    geometry->setLocal(
                        [] {
                            Transformation t;
                            t.rotate().fromAxisAngle( Vector3f::UNIT_X, -Numericf::HALF_PI );
                            t.setScale( 100.0f );
                            return t;
                        }() );
                    geometry->attachComponent< MaterialComponent >()->attachMaterial(
                        [] {
                            auto material = crimild::alloc< LitMaterial >();
                            material->setMetallic( 0.0f );
                            material->setRoughness( 1.0f );
                            return material;
                        }() );
                    return geometry;
                }() );

            scene->attachComponent< AudioSourceComponent >( AudioSystem::getInstance()->createAudioSource( FileSystem::getInstance().pathForResource( "assets/music/piano.ogg" ), true ) );
            scene->attachComponent< BackgroundMusic >();

            scene->perform( StartComponents() );

            return scene;
        }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Audio Room" );
