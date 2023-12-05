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
using namespace crimild::messaging;
using namespace crimild::import;

#if 0

class ViewControls
    : public NodeComponent,
      public Messenger {
public:
    ViewControls( void )
    {
        auto self = this;

        registerMessageHandler< MouseButtonDown >( [ self ]( MouseButtonDown const &msg ) {
            self->_lastMousePos = Input::getInstance()->getNormalizedMousePosition();
        } );

        registerMessageHandler< MouseMotion >( [ self ]( MouseMotion const &msg ) {
            if ( Input::getInstance()->isMouseButtonDown( CRIMILD_INPUT_MOUSE_BUTTON_LEFT ) ) {
                auto currentMousePos = Input::getInstance()->getNormalizedMousePosition();
                auto delta = currentMousePos - self->_lastMousePos;
                self->_lastMousePos = Input::getInstance()->getNormalizedMousePosition();

                if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT_SHIFT ) ) {
//                    self->translateView( Vector3f( 3.0f * delta[ 0 ], -3.0f * delta[ 1 ], 0.0f ) );
                } else {
//                    self->rotateView( Vector3f( delta[ 1 ], 3.0f * delta[ 0 ], 0.0f ) );
                }
            }
        } );

        registerMessageHandler< MouseButtonUp >( [ self ]( MouseButtonUp const &msg ) {

        } );

        registerMessageHandler< MouseScroll >( [ self ]( MouseScroll const &msg ) {
//            self->getNode()->local().translate() += 0.1f * msg.dy * Vector3f( 0.0f, 0.0f, 1.0f );
        } );
    }

    virtual ~ViewControls( void )
    {
    }

    virtual void update( const Clock &clock ) override
    {
        bool shouldTranslate = Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT_SHIFT );
        bool translateSpeed = 0.5f * clock.getDeltaTime();

        if ( Input::getInstance()->isKeyDown( 'W' ) ) {
            if ( shouldTranslate ) {
//                translateView( Vector3f( 0.0f, -translateSpeed, 0.0f ) );
            } else {
//                rotateView( Vector3f( -0.1f, 0.0f, 0.0f ) );
            }
        }

        if ( Input::getInstance()->isKeyDown( 'S' ) ) {
            if ( shouldTranslate ) {
//                translateView( Vector3f( 0.0f, translateSpeed, 0.0f ) );
            } else {
//                rotateView( Vector3f( 0.1f, 0.0f, 0.0f ) );
            }
        }

        if ( Input::getInstance()->isKeyDown( 'A' ) ) {
            if ( shouldTranslate ) {
//                translateView( Vector3f( translateSpeed, 0.0f, 0.0f ) );
            } else {
//                rotateView( Vector3f( 0.0f, -0.1f, 0.0f ) );
            }
        }

        if ( Input::getInstance()->isKeyDown( 'D' ) ) {
            if ( shouldTranslate ) {
//                translateView( Vector3f( -translateSpeed, 0.0f, 0.0f ) );
            } else {
//                rotateView( Vector3f( 0.0f, 0.1f, 0.0f ) );
            }
        }

        if ( Input::getInstance()->isKeyDown( 'Q' ) ) {
//            rotateView( Vector3f( 0.0f, 0.0f, 0.1f ) );
        }

        if ( Input::getInstance()->isKeyDown( 'E' ) ) {
//            rotateView( Vector3f( 0.0f, 0.0f, -0.1f ) );
        }
    }

private:
    void translateView( const Vector3f &delta )
    {
        getNode()->local().translate() += delta;
    }

    void rotateView( const Vector3f &delta )
    {
        if ( delta[ 0 ] != 0.0f ) {
            Vector3f xAxis( 1.0f, 0.0f, 0.0f );
            getNode()->getLocal().applyInverseToVector( Vector3f( 1.0f, 0.0f, 0.0f ), xAxis );
            getNode()->local().rotate() *= Quaternion4f::createFromAxisAngle( xAxis, delta[ 0 ] );
        }

        if ( delta[ 1 ] != 0.0f ) {
            Vector3f yAxis( 0.0f, 1.0f, 0.0f );
            getNode()->getLocal().applyInverseToVector( Vector3f( 0.0f, 1.0f, 0.0f ), yAxis );
            getNode()->local().rotate() *= Quaternion4f::createFromAxisAngle( yAxis, delta[ 1 ] );
        }

        if ( delta[ 2 ] != 0.0f ) {
            Vector3f zAxis( 0.0f, 0.0f, 1.0f );
            getNode()->getLocal().applyInverseToVector( Vector3f( 0.0f, 0.0f, 1.0f ), zAxis );
            getNode()->local().rotate() *= Quaternion4f::createFromAxisAngle( zAxis, delta[ 2 ] );
        }
    }

private:
    Vector2f _lastMousePos;
};

class LightControls : public NodeComponent {
public:
    LightControls( void )
    {
    }

    virtual ~LightControls( void )
    {
    }

    virtual void update( const Clock &clock ) override
    {
        float speed = 2.0f * clock.getDeltaTime();
        if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_UP ) ) {
            getNode()->local().translate() += speed * Vector3f( 0.0f, 1.0f, 0.0f );
        }

        if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_DOWN ) ) {
            getNode()->local().translate() += speed * Vector3f( 0.0f, -1.0f, 0.0f );
        }

        if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT ) ) {
            getNode()->local().translate() += speed * Vector3f( -1.0f, 0.0f, 0.0f );
        }

        if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_RIGHT ) ) {
            getNode()->local().translate() += speed * Vector3f( 1.0f, 0.0f, 0.0f );
        }

        getNode()->local().lookAt( Vector3f( 0.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) );
    }
};

class SceneControls : public NodeComponent,
                      public Messenger {
public:
    SceneControls( void )
    {
    }

    virtual ~SceneControls( void )
    {
    }

    virtual void start( void ) override
    {
        registerMessageHandler< KeyPressed >( []( KeyPressed const &msg ) {
            if ( msg.key == 'K' ) {
                MessageQueue::getInstance()->broadcastMessage( ToggleDebugInfo {} );
            }
        } );
    }
};

/*
int main( int argc, char **argv )
{
    auto sim = crimild::alloc< SDLSimulation >( "Crimild Model Viewer", crimild::alloc< Settings >( argc, argv ) );

    auto scene = crimild::alloc< Group >();

    auto camera = crimild::alloc< Camera >();
    auto renderPass = crimild::alloc< CompositeRenderPass >();
    renderPass->attachRenderPass( crimild::alloc< ShadowRenderPass >() );
    renderPass->attachRenderPass( crimild::alloc< StandardRenderPass >() );
    camera->setRenderPass( renderPass );
    // camera->setRenderPass( crimild::alloc< BasicRenderPass >() );
    scene->attachNode( camera );

    auto light = crimild::alloc< Light >( Light::Type::DIRECTIONAL );
    light->local().setTranslate( 1.0f, 1.0f, 1.0f );
    light->local().lookAt( Vector3f( 0.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) );
    // light->setAttenuation( Vector3f( 0.0f, 0.0f, 1.0f ) );
    // light->setShadowMap( crimild::alloc< ShadowMap >() );
    light->attachComponent< LightControls >();
    camera->attachNode( light );

    std::string modelPath = sim->getSettings()->get< std::string >( "file", "assets/models/ironman/Iron_Man.obj" );

    Clock c;
    c.tick();
    SharedPointer< Node > model;
    if ( StringUtils::getFileExtension( modelPath ) == ".crimild" ) {
        FileStream is( modelPath, FileStream::OpenMode::READ );
        is.load();
        if ( is.getObjectCount() > 0 ) {
            model = is.getObjectAt< Node >( 0 );
        }
    }
    else if ( StringUtils::getFileExtension( modelPath ) == ".obj" ) {
        OBJLoader loader( FileSystem::getInstance().pathForResource( modelPath ) );
        model = loader.load();
    }
    else {
        SceneImporter importer;
        model = importer.import( FileSystem::getInstance().pathForResource( modelPath ) );
    }
    c.tick();
    std::cout << "Time: " << c.getDeltaTime() << std::endl;

    if ( model != nullptr ) {
        // model->perform( SceneDebugDump( "dump.txt" ) );
        model->perform( UpdateWorldState() );

        // make sure the object is properly scaled
        float scale = 10.0f / model->getWorldBound()->getRadius();
        model->local().setScale( scale );
        model->local().translate() -= scale * Vector3f( 0.0f, model->getWorldBound()->getCenter()[ 1 ], 0.0f );

        auto pivot = crimild::alloc< Group >();
        pivot->attachNode( model );
        pivot->attachComponent< ViewControls >();
        scene->attachNode( pivot );

        camera->local().setTranslate( Vector3f( 0.0f, 0.0f, 15.0f ) );
    }
    else {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No model provided" );
        return -1;
    }

    scene->attachComponent< SceneControls >();

    sim->setScene( scene );
	return sim->run();
}
*/

#endif

using namespace crimild;

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
                    camera->setLocal( translation( 0, 10, 20 ) );
                    //                    camera->local().setTranslate( 0.0f, 10.0f, 20.0f );
                    //                    camera->local().lookAt( 5.0f * Vector3f::UNIT_Y );
                    camera->attachComponent< FreeLookCameraComponent >();
                    return camera;
                }()
            );

            scene->attachNode(
                [ & ] {
                    auto path = FilePath {
                        .pathType = settings->hasKey( "file" ) ? FilePath::PathType::ABSOLUTE : FilePath::PathType::RELATIVE,
                        .path = settings->get< std::string >( "file", "assets/models/ironman/Iron_Man.obj" ),
                    };

                    Clock c;
                    c.tick();
                    SharedPointer< Node > model;
                    if ( path.getExtension() == "crimild" ) {
                        /*
                        FileStream is( modelPath, FileStream::OpenMode::READ );
                        is.load();
                        if ( is.getObjectCount() > 0 ) {
                            model = is.getObjectAt< Node >( 0 );
                        }
                        */
                        exit( -1 );
                    } else if ( path.getExtension() == "obj" ) {
                        OBJLoader loader( path.getAbsolutePath() );
                        loader.setVerbose( true );
                        model = loader.load();
                    } else {
                        SceneImporter importer;
                        model = importer.import( path.getAbsolutePath() );
                    }
                    c.tick();
                    std::cout << "Time: " << c.getDeltaTime() << std::endl;

                    if ( model == nullptr ) {
                        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No model provided: ", path.getAbsolutePath() );
                        exit( -1 );
                    }

                    if ( settings->hasKey( "scale" ) ) {
                        auto scale = settings->get< Real32 >( "scale", 1.0f );
                        model->setLocal( crimild::scale( scale ) );
                    } else {
                        model->perform( UpdateWorldState() );
                        // make sure the object is properly scaled
                        auto scale = 10.0f / model->getWorldBound()->getRadius();
                        model->setLocal( crimild::scale( scale ) );

                        //                        Camera::getMainCamera()->local().lookAt( scale * model->getWorldBound()->getCenter() );
                    }
                    //                    return model;
                    auto pivot = crimild::alloc< Group >();
                    pivot->attachNode( model );
                    //         			return behaviors::withBehavior( pivot, behaviors::actions::rotate( Vector3 { 0, 1, 0 }, 0.1 ) );
                    return pivot;
                }()
            );

            scene->attachNode(
                [ & ] {
                    if ( !settings->hasKey( "skybox" ) ) {
                        return crimild::alloc< Skybox >( ColorRGB { 0.025f, 0.036f, 0.09f } );
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
                                    }
                                );
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
                        }()
                    );
                }()
            );

            scene->attachNode(
                [] {
                    auto light = crimild::alloc< Light >( Light::Type::DIRECTIONAL );
                    light->setColor( ColorRGBA::Constants::WHITE );
                    light->setEnergy( 10.0f );
                    light->setLocal(
                        lookAt(
                            Point3f { 1, 1, 1 },
                            Point3f { 0, 0, 0 },
                            Vector3::Constants::UP
                        )
                    );
                    //                    light->setCastShadows( true );
                    return light;
                }()
            );

            if ( settings->get< Bool >( "show.plane", true ) ) {
                scene->attachNode(
                    [] {
                        auto geometry = crimild::alloc< Geometry >();
                        geometry->attachPrimitive(
                            crimild::alloc< QuadPrimitive >(
                                QuadPrimitive::Params {}
                            )
                        );
                        geometry->setLocal(
                            [] {
                                Transformation t;
                                //                                t.rotate().fromAxisAngle( Vector3f::UNIT_X, -Numericf::HALF_PI );
                                //                                t.setScale( 100.0f );
                                return t;
                            }()
                        );
                        geometry->attachComponent< MaterialComponent >()->attachMaterial(
                            [] {
                                auto material = crimild::alloc< materials::PrincipledBSDF >();
                                material->setMetallic( 0.0f );
                                material->setRoughness( 1.0f );
                                return material;
                            }()
                        );
                        return geometry;
                    }()
                );
            }

            scene->perform( StartComponents() );

            return scene;
        }() );
    }
};

CRIMILD_CREATE_SIMULATION( Example, "Model Viewer" );
