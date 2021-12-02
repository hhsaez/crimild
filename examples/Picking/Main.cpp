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

SharedPointer< Node > makeSphere( float x, float y, float z )
{
	auto primitive = crimild::alloc< ParametricSpherePrimitive >( Primitive::Type::TRIANGLES, 1.0f );
	auto geometry = crimild::alloc< Geometry >();
	geometry->attachPrimitive( primitive );

	auto material = crimild::alloc< Material >();
	material->setDiffuse( RGBAColorf( 0.75f, 0.75f, 0.75f, 1.0f ) );
	geometry->getComponent< MaterialComponent >()->attachMaterial( material );
	geometry->attachComponent< UIResponder >( []( Node *node ) {
		MaterialComponent *materials = node->getComponent< MaterialComponent >();
		if ( materials ) {
			materials->forEachMaterial( []( Material *material ) {
				float r = rand() % 255 / 255.0f;
				float g = rand() % 255 / 255.0f;
				float b = rand() % 255 / 255.0f;
				material->setDiffuse( RGBAColorf( r, g, b, 1.0f ) );
			});
		}
		return true;
	});

	geometry->local().setTranslate( x, y, z );

	geometry->setName( "sphere" );

	return geometry;	
}

int main( int argc, char **argv )
{
	auto sim = crimild::alloc< sdl::SDLSimulation >( "Selecting objects with the mouse", crimild::alloc< Settings >( argc, argv ) );
	sim->addSystem( crimild::alloc< UISystem >() );

	auto scene = crimild::alloc< Group >();

	auto spheres = crimild::alloc< Group >();
	for ( float x = -5.0f; x <= 5.0f; x++ ) {
		for ( float y = -3.0f; y <= 3.0f; y++ ) {
			spheres->attachNode( makeSphere( x * 3.0f, y * 3.0f, 0.0f ) );
		}
	}
	auto rotationComponent = crimild::alloc< RotationComponent >( Vector3f( 0.0f, 1.0f, 0.0f ), 0.01f );
	spheres->attachComponent( rotationComponent );
	scene->attachNode( spheres );

	auto camera = crimild::alloc< Camera >( 45.0f, 4.0f / 3.0f, 0.1f, 1024.0f );
	camera->local().setTranslate( 10.0f, 15.0f, 50.0f );
	camera->local().setRotate( Vector3f( -1.0f, 0.5f, 0.0f ).getNormalized(), 0.1 * Numericf::PI );
	scene->attachNode( camera );
    
    auto light = crimild::alloc< Light >();
    light->local().setTranslate( 0.0f, 0.0f, 50.0f );
    scene->attachNode( light );

	sim->setScene( scene );
	return sim->run();
}

