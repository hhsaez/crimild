/*
 * Copyright (c) 2013, Hernan Saez
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

#include <Crimild.hpp>
#include <Crimild_SDL.hpp>
#include <Crimild_Import.hpp>
#include <Crimild_Scripting.hpp>

using namespace crimild;
using namespace crimild::animation;
using namespace crimild::messaging;
using namespace crimild::import;
using namespace crimild::sdl;

class ViewControls : 
    public NodeComponent,
    public Messenger {
public:
    ViewControls( void )
    {
        auto self = this;

        registerMessageHandler< MouseButtonDown >( [self]( MouseButtonDown const &msg ) {
            self->_lastMousePos = Input::getInstance()->getNormalizedMousePosition();
        });

        registerMessageHandler< MouseMotion >( [self]( MouseMotion const &msg ) {
            if ( Input::getInstance()->isMouseButtonDown( CRIMILD_INPUT_MOUSE_BUTTON_LEFT ) ) {
                auto currentMousePos = Input::getInstance()->getNormalizedMousePosition();
                auto delta = currentMousePos - self->_lastMousePos;
                self->_lastMousePos = Input::getInstance()->getNormalizedMousePosition();

                if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT_SHIFT ) ) {
                    self->translateView( Vector3f( 3.0f * delta[ 0 ], -3.0f * delta[ 1 ], 0.0f ) );
                }
                else {
                    self->rotateView( Vector3f( delta[ 1 ], 3.0f * delta[ 0 ], 0.0f ) );
                }
            }
        });

        registerMessageHandler< MouseButtonUp >( [self]( MouseButtonUp const &msg ) {

        });

        registerMessageHandler< MouseScroll >( [self]( MouseScroll const &msg ) {
            self->getNode()->local().translate() += 0.1f * msg.dy * Vector3f( 0.0f, 0.0f, 1.0f );
        });
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
                translateView( Vector3f( 0.0f, -translateSpeed, 0.0f ) );
            }
            else {
                rotateView( Vector3f( -0.1f, 0.0f, 0.0f ) );
            }
        }

        if ( Input::getInstance()->isKeyDown( 'S' ) ) {
            if ( shouldTranslate ) {
                translateView( Vector3f( 0.0f, translateSpeed, 0.0f ) );
            }
            else {
                rotateView( Vector3f( 0.1f, 0.0f, 0.0f ) );
            }
        }

        if ( Input::getInstance()->isKeyDown( 'A' ) ) {
            if ( shouldTranslate ) {
                translateView( Vector3f( translateSpeed, 0.0f, 0.0f ) );
            }
            else {
                rotateView( Vector3f( 0.0f, -0.1f, 0.0f ) );
            }
        }

        if ( Input::getInstance()->isKeyDown( 'D' ) ) {
            if ( shouldTranslate ) {
                translateView( Vector3f( -translateSpeed, 0.0f, 0.0f ) );
            }
            else {
                rotateView( Vector3f( 0.0f, 0.1f, 0.0f ) );
            }
        }

        if ( Input::getInstance()->isKeyDown( 'Q' ) ) {
            rotateView( Vector3f( 0.0f, 0.0f, 0.1f ) );
        }

        if ( Input::getInstance()->isKeyDown( 'E' ) ) {
            rotateView( Vector3f( 0.0f, 0.0f, -0.1f ) );
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
    }
};

class SceneControls : 
    public NodeComponent,
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
        });
    }
};

class DebugGeometryInfo : public NodeComponent {
public:
	DebugGeometryInfo( void ) { }
	virtual ~DebugGeometryInfo( void ) { }

	virtual void renderDebugInfo( Renderer *renderer, Camera *camera ) override
	{
		getNode()->perform( ApplyToGeometries( [ renderer, camera ]( Geometry *geometry ) {
			auto bounds = geometry->getWorldBound();
			auto center = bounds->getCenter();
			auto size = bounds->getMax() - bounds->getMin();

			DebugRenderHelper::renderBox( renderer, camera, center, size, RGBAColorf( 1.0f, 0.0f, 1.0f, 0.25f ) );
			DebugRenderHelper::renderSphere( renderer, camera, center, 0.1f, RGBAColorf( 1.0f, 1.0f, 0.0f, 0.5f ) );
		}));
	}
};

SharedPointer< Node > convert( std::string file )
{
    Clock c;

    c.tick();

    SceneImporter importer;
    auto model = importer.import( FileSystem::getInstance().pathForResource( file ) );
    if ( model == nullptr ) {
        return nullptr;
    }

    c.tick();
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Loaded raw model: ", c.getDeltaTime(), "s" );

    {
        coding::FileEncoder encoder;
		encoder.encode( model );
		
		if ( !encoder.write( FileSystem::getInstance().pathForResource( "assets/model.crimild" ) ) ) {
			return nullptr;
		}
    }
	
    c.tick();
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Encoded to file: ", c.getDeltaTime(), "s" );

	{
		coding::FileDecoder decoder;
		if ( !decoder.read( FileSystem::getInstance().pathForResource( "assets/model.crimild" ) ) ) {
			return nullptr;
		}
		
		if ( decoder.getObjectCount() == 0 ) {
			Log::error( "File is empty?" );
			return nullptr;
		}

		model = decoder.getObjectAt< Group >( 0 );
	}

	c.tick();
	Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Decoded from file: ", c.getDeltaTime(), "s" );

	model->attachComponent< DebugGeometryInfo >();

	if ( auto skeleton = model->getComponent< Skeleton >() ) {
		if ( skeleton->getClips().size() > 0 ) {
			auto animation = crimild::alloc< Animation >( skeleton->getClips().values().first() );
			model->attachComponent< LambdaComponent >( [ animation, skeleton ]( Node *, const Clock &c ) {
				animation->update( c );
				skeleton->animate( animation );
			});
		}
	}
	
    return model;
}

int main( int argc, char **argv )
{
    crimild::init();
    
    auto sim = crimild::alloc< SDLSimulation >( "Crimild Model Converter", crimild::alloc< Settings >( argc, argv ) );
	auto renderer = sim->getRenderer();
	renderer->getScreenBuffer()->setClearColor( RGBAColorf( 0.5f, 0.5f, 0.5f, 0.0f ) );

    auto scene = crimild::alloc< Group >();

    auto camera = crimild::alloc< Camera >();
    // camera->setRenderPass( crimild::alloc< BasicRenderPass >() );
    scene->attachNode( camera );

    auto light = crimild::alloc< Light >();
    light->local().lookAt( Vector3f( 0.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) );
    light->attachComponent< LightControls >();
    camera->attachNode( light );

    std::string modelPath = sim->getSettings()->get< std::string >( "file", "assets/models/astroboy/astroBoy_walk_Max.dae" );
    auto model = convert( modelPath );
    if ( model != nullptr ) {

        model->perform( Apply( []( Node *node ) {
            auto cmp = node->getComponent< SkinnedMeshComponent >();
            if ( cmp != nullptr ) {
                cmp->getSkinnedMesh()->debugDump();
            }
        }));

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
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "No model" );
        return -1;
    }

    scene->attachComponent< SceneControls >();

	sim->registerMessageHandler< crimild::messaging::KeyReleased >( []( crimild::messaging::KeyReleased const &msg ) {
		switch ( msg.key ) {
			case 'K':
				Simulation::getInstance()->broadcastMessage( crimild::messaging::DebugModeEnabled { } );
				break;
			case 'L':
				Simulation::getInstance()->broadcastMessage( crimild::messaging::DebugModeDisabled { } );
				break;
		}
	});

    sim->setScene( scene );
	return sim->run();
}

