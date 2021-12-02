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

#include <fstream>
#include <string>
#include <vector>

using namespace crimild;
using namespace crimild::audio;
using namespace crimild::sdl;

class DroneComponent : public NodeComponent {
public:
	DroneComponent( void ) { }
	virtual ~DroneComponent( void ) { }

	virtual void onAttach( void ) override
	{
		_height = Random::generate< crimild::Real32 >( 1.0f, 16.0f );

		getNode()->local().setTranslate( getRandomPosition() );

		_target = getRandomPosition();
	}

	virtual void update( const Clock &c ) override
	{
		auto d = Distance::compute( getNode()->getWorld().getTranslate(), _target );
		if ( d < 1.0f ) {
			_target = getRandomPosition();
		}

		getNode()->local().lookAt( _target );
		getNode()->local().translate() += 10.0f * c.getDeltaTime() * getNode()->local().computeDirection();
	}

private:
	Vector3f getRandomPosition( void ) const 
	{
		return Random::generate< Vector3f >( Vector3f( -18.0f, _height, 0.0f ), Vector3f( 18.0f, _height, -50.0f ) );	
	}

private:	
	Vector3f _target;
	crimild::Real32 _height = 10.0f;
};

SharedPointer< Node > loadDrone( void )
{
    auto drone = crimild::alloc< Group >( "drone" );
    
	OBJLoader loader( FileSystem::getInstance().pathForResource( "assets/models/drone/MQ-27b.obj" ) );
	auto droneModel = loader.load();
	if ( droneModel != nullptr ) {
		droneModel->local().rotate().fromEulerAngles( 0.0f, -Numericf::HALF_PI, 0.0f );
		drone->attachNode( droneModel );
        
		auto droneComponent = crimild::alloc< DroneComponent >();
		drone->attachComponent( droneComponent );
        
		auto audioSource = AudioManager::getInstance()->createAudioSource( FileSystem::getInstance().pathForResource( "assets/audio/drone_mono.wav" ), false );
		audioSource->setLoop( true );
		audioSource->setAutoplay( true );
		audioSource->enableSpatialization( true );
		audioSource->setMinDistance( 5.0f );
		drone->attachComponent< AudioSourceComponent >( audioSource );
	}
    
    return drone;
}

SharedPointer< Node > makeGround( void )
{
	auto primitive = crimild::alloc< QuadPrimitive >( 200.0f, 200.0f );
	auto geometry = crimild::alloc< Geometry >();
	geometry->attachPrimitive( primitive );
	geometry->local().setRotate( Vector3f( 1.0f, 0.0f, 0.0f ), -Numericf::HALF_PI );
    geometry->local().setTranslate( 0.0f, 0.0f, -30.0f );
	
	return geometry;
}

SharedPointer< Node > loadRoom( void )
{
	OBJLoader loader( FileSystem::getInstance().pathForResource( "assets/models/room/room.obj" ) );
	auto model = loader.load();
	return model;
}

int main( int argc, char **argv )
{
	auto sim = crimild::alloc< SDLSimulation >( "Drone", crimild::alloc< Settings >( argc, argv ) );

	auto scene = crimild::alloc< Group >();
	for ( int i = 0; i < 3; i++ ) {
    	scene->attachNode( loadDrone() );
    }
	scene->attachNode( loadRoom() );

	auto light = crimild::alloc< Light >( Light::Type::POINT );
	light->local().setTranslate( 0.0f, 10.0f, -20.0f );
    // light->local().lookAt( Vector3f( 0.0f, 0.0f, -8.0f ), Vector3f( 0.0f, 1.0f, 0.0 ) );
    // light->setShadowMap( crimild::alloc< ShadowMap >() );
	scene->attachNode( light );

	auto camera = crimild::alloc< Camera >( 45.0f, 4.0f / 3.0f, 0.1f, 1024.0f );
	camera->attachComponent< AudioListenerComponent >();
	camera->attachComponent< FreeLookCameraComponent >();
	camera->local().setTranslate( 0.0f, 6.0f, 15.0f );
    auto renderPass = crimild::alloc< CompositeRenderPass >();
    renderPass->attachRenderPass( crimild::alloc< ShadowRenderPass >() );
    renderPass->attachRenderPass( crimild::alloc< StandardRenderPass >() );
    camera->setRenderPass( renderPass );
	scene->attachNode( camera );

	sim->setScene( scene );
	return sim->run();
}

