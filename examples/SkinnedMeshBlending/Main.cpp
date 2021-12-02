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

using namespace crimild;
using namespace crimild::animation;
using namespace crimild::messaging;
using namespace crimild::import;
using namespace crimild::sdl;

class AnimateWithBlending : public NodeComponent {
	CRIMILD_IMPLEMENT_RTTI( AnimateWithBlending )
	
public:
	AnimateWithBlending( void ) { }
	virtual ~AnimateWithBlending( void ) { }
	
	virtual void start( void ) override
	{
		auto skeleton = getComponent< Skeleton >();
		
		_idle = crimild::alloc< Animation >( skeleton->getClips()[ "Wolf_Skeleton|Wolf_Idle_" ] );
		_creep = crimild::alloc< Animation >( skeleton->getClips()[ "Wolf_Skeleton|Wolf_creep_cycle" ] );
		_walk = crimild::alloc< Animation >( skeleton->getClips()[ "Wolf_Skeleton|Wolf_Walk_cycle_" ] );
		_run = crimild::alloc< Animation >( skeleton->getClips()[ "Wolf_Skeleton|Wolf_Run_Cycle_" ] );
	}

	virtual void update( const Clock &c ) override
	{
		const crimild::Real32 creepSpeed = 1.0f;
		const crimild::Real32 walkSpeed = 4.0f;
		const crimild::Real32 runSpeed = 9.0f;
		
		if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT ) ) {
			_speed = Numericf::max( 0.0f, _speed - c.getDeltaTime() );
		}
		if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_RIGHT ) ) {
			_speed = Numericf::min( runSpeed, _speed + c.getDeltaTime() );
		}
		
		auto alpha = Numericf::clamp( _speed / creepSpeed, 0.0f, 1.0f );
		auto beta = Numericf::clamp( ( _speed - creepSpeed ) / ( walkSpeed - creepSpeed ), 0.0f, 1.0f );
		auto gamma = Numericf::clamp( ( _speed - walkSpeed ) / ( runSpeed - walkSpeed ), 0.0f, 1.0f );
		
		std::stringstream ss;
		ss << "SPEED: " << _speed
		   << "\nIDLE: " << ( 1.0f - alpha )
		   << "\nSTALK: " << ( alpha * ( 1.0f - beta ) )
		   << "\nWALK: " << ( beta * ( 1.0f - gamma ) )
		   << "\nRUN: " << gamma;
		_description = ss.str();
		
		auto anim = _idle->update( c )->lerp( _creep, alpha )->lerp( _walk, beta )->lerp( _run, gamma );
		
		getComponent< Skeleton >()->animate( anim );
	}

	std::string getDescription( void ) const { return _description; }

private:
	crimild::Real32 _speed = 0.0f;
	
	SharedPointer< Animation > _idle;
	SharedPointer< Animation > _creep;
	SharedPointer< Animation > _walk;
	SharedPointer< Animation > _run;

	std::string _description;
};

class AnimateWithoutBlending : public NodeComponent {
public:
	AnimateWithoutBlending( void ) { }
	virtual ~AnimateWithoutBlending( void ) { }

	virtual void start( void ) override
	{
		auto skeleton = getComponent< Skeleton >();
		
		_idle = crimild::alloc< Animation >( skeleton->getClips()[ "Wolf_Skeleton|Wolf_Idle_" ] );
		_creep = crimild::alloc< Animation >( skeleton->getClips()[ "Wolf_Skeleton|Wolf_creep_cycle" ] );
		_walk = crimild::alloc< Animation >( skeleton->getClips()[ "Wolf_Skeleton|Wolf_Walk_cycle_" ] );
		_run = crimild::alloc< Animation >( skeleton->getClips()[ "Wolf_Skeleton|Wolf_Run_Cycle_" ] );
	}

	virtual void update( const Clock &c ) override
	{
		const crimild::Real32 creepSpeed = 1.0f;
		const crimild::Real32 walkSpeed = 4.0f;
		const crimild::Real32 runSpeed = 9.0f;
		
		if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT ) ) {
			_speed = Numericf::max( 0.0f, _speed - c.getDeltaTime() );
		}
		if ( Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_RIGHT ) ) {
			_speed = Numericf::min( runSpeed, _speed + c.getDeltaTime() );
		}

		Animation *anim = crimild::get_ptr( _idle );
		if ( _speed > 0 ) {
			if ( _speed <= 1.5f * creepSpeed ) {
				anim = crimild::get_ptr( _creep );
			}
			else if ( _speed <= 1.5f * walkSpeed ) {
				anim = crimild::get_ptr( _walk );
			}
			else if ( _speed <= runSpeed ) {
				anim = crimild::get_ptr( _run );
			}
		}

		anim->update( c );
		getComponent< Skeleton >()->animate( anim );
	}

private:
	crimild::Real32 _speed = 0.0f;
	
	SharedPointer< Animation > _idle;
	SharedPointer< Animation > _creep;
	SharedPointer< Animation > _walk;
	SharedPointer< Animation > _run;
};

SharedPointer< Node > loadWolf( void )
{
	auto fileName = std::string( "assets/models/wolf/Wolf.fbx" );
	
	if ( AssetManager::getInstance()->get< Group >( fileName ) == nullptr ) {
		SceneImporter importer;
		auto model = importer.import( FileSystem::getInstance().pathForResource( "assets/models/wolf/Wolf.fbx" ) );
		AssetManager::getInstance()->set( fileName, model );
	}

	auto model = AssetManager::getInstance()->get< Group >( fileName );

	ShallowCopy copy;
	model->perform( copy );
	auto wolf = copy.getResult< Group >();
	
	return wolf;
}

int main( int argc, char **argv )
{
    auto sim = crimild::alloc< SDLSimulation >( "Sninned Mesh: Blending animations", crimild::alloc< Settings >( argc, argv ) );
	sim->getRenderer()->getScreenBuffer()->setClearColor( RGBAColorf( 0.5f, 0.5f, 0.75f, 0.0f ) );

    auto scene = crimild::alloc< Group >();

    auto camera = crimild::alloc< Camera >();
	camera->local().setTranslate( -30.0f, 30.0f, 100.0f );
	camera->local().rotate().fromEulerAngles( 0.0f, -0.15f * Numericf::PI, 0.0f );
    scene->attachNode( camera );

	auto text = crimild::alloc< Text >();
	text->setFont( crimild::alloc< Font >( FileSystem::getInstance().pathForResource( "assets/fonts/Courier New.txt" ) ) );
	text->setSize( 0.05f );
	text->setTextColor( RGBAColorf( 1.0f, 1.0f, 0.0f, 1.0f ) );
	text->local().setTranslate( -0.75f, 0.5f, -1.0f );
	text->setHorizontalAlignment( Text::HorizontalAlignment::LEFT );
	text->setText( "" );
	camera->attachNode( text );

    auto light = crimild::alloc< Light >( Light::Type::DIRECTIONAL );
    light->local().setTranslate( 0.0f, 0.0f, 0.0f );
    camera->attachNode( light );

	auto wolf1 = loadWolf();
	wolf1->attachComponent< AnimateWithBlending >();
	wolf1->local().setTranslate( 0.0f, 0.0f, 0.0f );
	wolf1->attachComponent< LambdaComponent >( [ text ]( Node *node, const Clock & ) {
		auto animator = node->getComponent< AnimateWithBlending >();
		text->setText( animator->getDescription() );
	});
	scene->attachNode( wolf1 );

	auto wolf2 = loadWolf();
	wolf2->attachComponent< AnimateWithoutBlending >();
	wolf2->local().setTranslate( 50.0f, 0.0f, 0.0f );
	scene->attachNode( wolf2 );

	sim->registerMessageHandler< KeyPressed >( []( KeyPressed const &msg ) {
		if ( msg.key == 'K' ) {
			MessageQueue::getInstance()->broadcastMessage( ToggleDebugInfo {} );
		}
	});

    sim->setScene( scene );
	
	return sim->run();
}

