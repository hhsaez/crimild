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
#include <functional>

using namespace crimild;
using namespace crimild::sdl;

int main( int argc, char **argv )
{
	auto sim = crimild::alloc< SDLSimulation >( "Interpolations", crimild::alloc< Settings >( argc, argv ) );

	auto trefoilKnot = crimild::alloc< Geometry >();
	auto trefoilKnotPrimitive = crimild::alloc< TrefoilKnotPrimitive >( Primitive::Type::TRIANGLES, 1.0, VertexFormat::VF_P3_N3 );
	trefoilKnot->attachPrimitive( trefoilKnotPrimitive );

	Vector3f from( -3.0f, -1.0f, 0.0f );
	Vector3f to( 3.0f, 2.0f, 0.0f );
	float functTime = 0.0f;
	std::map< std::string, std::function< void ( const float &, const float &, float, float & ) > > easingFunctions;
	easingFunctions[ "linear" ] = Interpolation::linear< float, float >;
	easingFunctions[ "cuadraticIn" ] = Interpolation::cuadraticIn< float, float >;
	easingFunctions[ "cubicIn" ] = Interpolation::cubicIn< float, float >;
	easingFunctions[ "cubicOut" ] = Interpolation::cubicOut< float, float >;
	easingFunctions[ "cubicInOut" ] = Interpolation::cubicInOut< float, float >;
	easingFunctions[ "cubicOutIn" ] = Interpolation::cubicOutIn< float, float >;
	easingFunctions[ "cubicBackIn" ] = Interpolation::cubicBackIn< float, float >;
	easingFunctions[ "cubicBackOut" ] = Interpolation::cubicBackOut< float, float >;
	easingFunctions[ "cuarticIn" ] = Interpolation::cuarticIn< float, float >;
	easingFunctions[ "cuarticOut" ] = Interpolation::cuarticOut< float, float >;
	easingFunctions[ "cuarticOutIn" ] = Interpolation::cuarticOutIn< float, float >;
	easingFunctions[ "cuarticBackIn" ] = Interpolation::cuarticBackIn< float, float >;
	easingFunctions[ "cuarticBackOut" ] = Interpolation::cuarticBackOut< float, float >;
	easingFunctions[ "quinticIn" ] = Interpolation::quinticIn< float, float >;
	easingFunctions[ "quinticOut" ] = Interpolation::quinticOut< float, float >;
	easingFunctions[ "quinticInOut" ] = Interpolation::quinticInOut< float, float >;
	easingFunctions[ "elasticInSmall" ] = Interpolation::elasticInSmall< float, float >;
	easingFunctions[ "elasticInBig" ] = Interpolation::elasticInBig< float, float >;
	easingFunctions[ "elasticOutSmall" ] = Interpolation::elasticOutSmall< float, float >;
	easingFunctions[ "elasticOutBig" ] = Interpolation::elasticOutBig< float, float >;
	auto funcIt = easingFunctions.begin();
	auto updateCmp = crimild::alloc< LambdaComponent >( [&]( Node *node, const Clock &appTime ) {
		float x, y;

		float t = Numericf::min( Numericf::max( functTime, 0.0 ), 1.0 );

		Interpolation::linear( from.x(), to.x(), t, x );

		std::string funcName = funcIt->first;
		std::cout << funcName << std::endl;
		funcIt->second( from.y(), to.y(), t, y );

		node->local().setTranslate( x, y, 0.0 );

		functTime += appTime.getDeltaTime();
		if ( functTime > 1.5f ) {
			funcIt++;
			if ( funcIt == easingFunctions.end() ) {
				funcIt = easingFunctions.begin();
			}
			functTime = -0.5f;
		}
	});
	trefoilKnot->attachComponent( updateCmp );

	auto scene = crimild::alloc< Group >();
	scene->attachNode( trefoilKnot );

	auto light = crimild::alloc< Light >();
	light->local().setTranslate( 0.0f, 0.0f, 10.0f );
	scene->attachNode( light );

	auto camera = crimild::alloc< Camera >();
	camera->local().setTranslate( 0.0f, 0.0f, 10.0f );
	scene->attachNode( camera );

	sim->setScene( scene );
	return sim->run();
}

