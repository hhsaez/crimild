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

using namespace crimild;
using namespace crimild::navigation;

SharedPointer< Node > createEnvironment( void )
{
	auto group = crimild::alloc< Group >();

	OBJLoader loader( FileSystem::getInstance().pathForResource( "assets/models/level.obj" ) );
	auto node = loader.load();
	if ( node != nullptr ) {
		group->attachNode( node );
	}

	auto light = crimild::alloc< Light >();
	light->local().setTranslate( 10.0f, 10.0f, 10.0f );
	light->local().lookAt( Vector3f::ZERO );
    light->setShadowMap( crimild::alloc< ShadowMap >() );
	group->attachNode( light );

	return group;
}

SharedPointer< Node > createCharacter( void )
{
	auto group = crimild::alloc< Group >();
	
	OBJLoader loader( FileSystem::getInstance().pathForResource( "assets/models/character.obj" ) );
	auto node = loader.load();
	if ( node != nullptr ) {
		group->attachNode( node );
	}

	group->attachComponent< LambdaComponent >( []( Node *node, const Clock &c ) {
		const auto SPEED = 2.0f * c.getDeltaTime();

		auto pos = node->getLocal().getTranslate();

		auto angle = 0.0f;
		
		auto h = Input::getInstance()->getAxis( Input::AXIS_HORIZONTAL );
		auto v = Input::getInstance()->getAxis( Input::AXIS_VERTICAL );

		if ( h > 0 ) angle = Numericf::HALF_PI;
		if ( h < 0 ) angle = -Numericf::HALF_PI;
		if ( v > 0 ) angle = Numericf::PI;

		pos += SPEED * ( h * Vector3f::UNIT_X + v * -Vector3f::UNIT_Z );

		auto nav = node->getComponent< NavigationController >();
		if ( nav != nullptr ) {
			nav->move( pos );
		}

		node->local().rotate().fromEulerAngles( 0.0f, angle, 0.0f );
	});
	
	return group;
}

int main( int argc, char **argv )
{
    auto sim = crimild::alloc< sdl::SDLSimulation >( "Navigation", crimild::alloc< Settings >( argc, argv ) );
    sim->getRenderer()->getScreenBuffer()->setClearColor( RGBAColorf( 0.25f, 0.25f, 0.25f, 1.0f ) );

	AssetManager::getInstance()->loadFont( AssetManager::FONT_SYSTEM, "assets/fonts/Courier New.txt" );
	Profiler::getInstance()->setOutputHandler( crimild::alloc< ProfilerScreenOutputHandler >() );

	sim->registerMessageHandler< crimild::messaging::KeyPressed >( []( crimild::messaging::KeyPressed const &msg ) {
		const float speedCoeff = Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_LEFT_SHIFT ) ? 3.0f : 1.0f;

		switch ( msg.key ) {
			case CRIMILD_INPUT_KEY_LEFT:
				Input::getInstance()->setAxis( Input::AXIS_HORIZONTAL, -speedCoeff );
				break;

			case CRIMILD_INPUT_KEY_RIGHT:
				Input::getInstance()->setAxis( Input::AXIS_HORIZONTAL, +speedCoeff );
				break;

			case CRIMILD_INPUT_KEY_UP:
				Input::getInstance()->setAxis( Input::AXIS_VERTICAL, +speedCoeff );
				break;

			case CRIMILD_INPUT_KEY_DOWN:
				Input::getInstance()->setAxis( Input::AXIS_VERTICAL, -speedCoeff );
				break;

			default:
				break;
		}
	});

	sim->registerMessageHandler< crimild::messaging::KeyReleased >( []( crimild::messaging::KeyReleased const &msg ) {
		switch ( msg.key ) {
			case CRIMILD_INPUT_KEY_K:
				Simulation::getInstance()->broadcastMessage( crimild::messaging::ToggleDebugInfo { } );
				break;

			case CRIMILD_INPUT_KEY_L:
				Simulation::getInstance()->broadcastMessage( crimild::messaging::ToggleProfilerInfo { } );
				break;
				
			case CRIMILD_INPUT_KEY_LEFT:
			case CRIMILD_INPUT_KEY_RIGHT:
				Input::getInstance()->setAxis( Input::AXIS_HORIZONTAL, 0.0f );
				break;

			case CRIMILD_INPUT_KEY_UP:
			case CRIMILD_INPUT_KEY_DOWN:
				Input::getInstance()->setAxis( Input::AXIS_VERTICAL, 0.0f );
				break;

			default:
				break;
		}
	});

    auto scene = crimild::alloc< Group >();

	auto navigationMesh = crimild::alloc< NavigationMeshOBJ >( FileSystem::getInstance().pathForResource( "assets/models/level_navmesh.obj" ) );

	auto environment = createEnvironment();
	environment->attachComponent< NavigationMeshContainer >( navigationMesh );
	scene->attachNode( environment );

	auto character = createCharacter();
	character->attachComponent< NavigationController >( navigationMesh );
	scene->attachNode( character );

    auto camera = crimild::alloc< Camera >();
    camera->local().setTranslate( Vector3f( 0.0f, 15.0f, 15.0f ) );
	camera->local().lookAt( Vector3f::ZERO, Vector3f::UNIT_Y );
    auto renderPass = crimild::alloc< CompositeRenderPass >();
    renderPass->attachRenderPass( crimild::alloc< ShadowRenderPass >() );
    renderPass->attachRenderPass( crimild::alloc< StandardRenderPass >() );
    camera->setRenderPass( renderPass );
    scene->attachNode( camera );
    
    sim->setScene( scene );
	return sim->run();
}

