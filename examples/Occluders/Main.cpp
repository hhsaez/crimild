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
	auto sim = crimild::alloc< SDLSimulation >( "Default shapes", crimild::alloc< Settings >( argc, argv ) );

	auto scene = crimild::alloc< Group >();

	auto background = crimild::alloc< Geometry >();
	background->attachPrimitive( crimild::alloc< QuadPrimitive >( 10.0f, 10.0f, VertexFormat::VF_P3_N3 ) );
	background->local().setTranslate( 0.0f, 0.0f, -10.0f );
	auto backgroundMaterial = crimild::alloc< Material >();
	backgroundMaterial->setDiffuse( RGBAColorf( 0.25f, 1.0f, 0.25f, 1.0f ) );
	background->getComponent< MaterialComponent >()->attachMaterial( backgroundMaterial );
	scene->attachNode( background );

	auto shapes = crimild::alloc< Group >();
	scene->attachNode( shapes );

	auto kleinBottle = crimild::alloc< Geometry >();
	auto kleinBottlePrimitive = crimild::alloc< KleinBottlePrimitive >( Primitive::Type::TRIANGLES, 0.1 );
	kleinBottle->attachPrimitive( kleinBottlePrimitive );
	kleinBottle->local().setTranslate( 0.0f, 0.0f, 3.0f );
	shapes->attachNode( kleinBottle );

	auto mobiusStrip = crimild::alloc< Geometry >();
	auto mobiusStripPrimitive = crimild::alloc< MobiusStripPrimitive >( Primitive::Type::TRIANGLES, 0.5f );
	mobiusStrip->attachPrimitive( mobiusStripPrimitive );
	mobiusStrip->local().setTranslate( 0.0f, 0.0f, -3.0f );
	shapes->attachNode( mobiusStrip );

	auto torus = crimild::alloc< Geometry >();
	auto torusPrimitive = crimild::alloc< TorusPrimitive >( Primitive::Type::TRIANGLES, 1.0f, 0.25f );
	torus->attachPrimitive( torusPrimitive );
	torus->local().setTranslate( 3.0f, 0.0f, 0.0f );
	shapes->attachNode( torus );

	auto trefoilKnot = crimild::alloc< Geometry >();
	auto trefoilKnotPrimitive = crimild::alloc< TrefoilKnotPrimitive >( Primitive::Type::TRIANGLES, 1.0 );
	trefoilKnot->attachPrimitive( trefoilKnotPrimitive );
	trefoilKnot->local().setTranslate( -3.0f, 0.0f, 0.0f );
	shapes->attachNode( trefoilKnot );

	auto rotate = crimild::alloc< RotationComponent >( Vector3f( 0.0f, 1.0f, 0.0f ), 0.1f );
	shapes->attachComponent( rotate );

	auto teapot = crimild::alloc< Geometry >();
	teapot->attachPrimitive( crimild::alloc< NewellTeapotPrimitive >() );
	teapot->local().setTranslate( 0.0f, -1.0f, 0.0f );
	teapot->local().setScale( 0.1f );
	auto occluderMaterial = crimild::alloc< Material >();
	occluderMaterial->setDiffuse( RGBAColorf( 1.0f, 1.0f, 0.0f, 1.0f ) );
	occluderMaterial->setColorMaskState( ColorMaskState::DISABLED );
	teapot->getComponent< MaterialComponent >()->attachMaterial( occluderMaterial );
	teapot->attachComponent< LambdaComponent >( [occluderMaterial]( Node *, const Clock & ) {
		bool enabled = Input::getInstance()->isKeyDown( CRIMILD_INPUT_KEY_SPACE );
		occluderMaterial->getColorMaskState()->setEnabled( enabled );
	});
	scene->attachNode( teapot );

	// auto trefoilKnot2 = crimild::alloc< Geometry >();
	// trefoilKnot2->attachPrimitive( crimild::alloc< TrefoilKnotPrimitive >( Primitive::Type::TRIANGLES, 1.0 ) );
	// trefoilKnot2->local().setTranslate( 0.0f, 0.0f, 0.0f );
	// trefoilKnot2->attachComponent< LambdaComponent >( []( Node *n, const Clock &c ) {
	// 	static float t = 0.0f;
	// 	t += c.getDeltaTime();
	// 	n->local().setTranslate( 0.0f, 0.0f, 5.0f * Numericf::sin( t ) );
	// });
	// scene->attachNode( trefoilKnot2 );

	auto light = crimild::alloc< Light >();
	light->local().setTranslate( 0.0f, 0.0f, 10.0f );
	scene->attachNode( light );

	auto camera = crimild::alloc< Camera >();
	camera->local().setTranslate( 2.0f, 2.0f, 5.0f );
	camera->local().lookAt( Vector3f::ZERO );
	scene->attachNode( camera );

	sim->getRenderer()->getScreenBuffer()->setClearColor( RGBAColorf( 0.95f, 0.25f, 0.25f, 0.0f ) );

	sim->setScene( scene );
	
	return sim->run();
}

