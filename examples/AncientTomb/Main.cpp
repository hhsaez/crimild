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
using namespace crimild::sdl;

int main( int argc, char **argv )
{
	auto sim = crimild::alloc< SDLSimulation >( "Ancient Tomb", crimild::alloc< Settings >( argc, argv ) );

	auto scene = crimild::alloc< Group >();

	OBJLoader loader( FileSystem::getInstance().pathForResource( "assets/tomb.obj" ) );
	auto model = loader.load();
	if ( model != nullptr ) {
		auto rotationComponent = crimild::alloc< RotationComponent >( Vector3f( 0, 1, 0 ), 0.05 );
		model->attachComponent( rotationComponent );
		scene->attachNode( model );

        auto light1 = crimild::alloc< Light >();
        light1->local().setTranslate( -9.0f, 5.0f, 0.0f );
        light1->local().lookAt( Vector3f( 0.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) );
        //light1->setCastShadows( true );
        //light1->setShadowNearCoeff( 0.1f );
        //light1->setShadowFarCoeff( 20.0f );
        light1->setAttenuation( Vector3f( .1f, 0.0f, 0.005f ) );
        model->attachNode( light1 );
        
        auto light2 = crimild::alloc< Light >();
        light2->local().setTranslate( -4.0f, 0.5f, 8.0f );
        light2->setColor( RGBAColorf( 0.0f, 0.0f, 1.0f, 1.0f ) );
        light2->setAttenuation( Vector3f( .1f, 0.0f, 0.05f ) );
        model->attachNode( light2 );

        auto light3 = crimild::alloc< Light >();
        light3->local().setTranslate( -4.0f, 0.5f, -12.0f );
        light3->setColor( RGBAColorf( 1.0f, 0.0f, 0.0f, 1.0f ) );
        light3->setAttenuation( Vector3f( .1f, 0.0f, 0.025f ) );
        model->attachNode( light3 );
    }

	auto camera = crimild::alloc< Camera >( 60.0f, 4.0f / 3.0f, 0.1f, 1024.0f );
	camera->local().setTranslate( 0.0f, 2.0f, 6.0f );
    camera->local().lookAt( Vector3f( 0.0f, 2.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) );
    camera->setCullingEnabled( false );
	scene->attachNode( camera );

	sim->setScene( scene );

	return sim->run();
}

