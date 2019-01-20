/*
 * Copyright (c) 2002-present, H. Hernan Saez
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

#include "ScreenTextureShaderProgram.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/CSL.hpp"
#include "Rendering/ShaderGraph/Nodes/UnlitFragmentMaster.hpp"

using namespace crimild;
using namespace crimild::shadergraph;
using namespace crimild::shadergraph::csl;

ScreenTextureShaderProgram::ScreenTextureShaderProgram( Mode mode )
	: _mode( mode )
{
	createVertexShader();
	createFragmentShader();
}

ScreenTextureShaderProgram::~ScreenTextureShaderProgram( void )
{

}

void ScreenTextureShaderProgram::createVertexShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto p = csl::screenPosition();
	auto uv = csl::modelTextureCoords();

	csl::vertexOutput( "vTextureCoord", uv );
	csl::vertexPosition( p );

	buildVertexShader( graph );
}

void ScreenTextureShaderProgram::createFragmentShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto master = graph->addNode< UnlitFragmentMaster >();

	auto uv = csl::vec2_in( "vTextureCoord" );
	auto texture = csl::colorMapUniform();
	auto color = csl::textureColor( texture, uv );

	switch ( _mode ) {
		case Mode::RED: {
			auto r = csl::vec_x( color );
			color = csl::vec4( r, r, r, csl::scalar_one() );
			break;
		}

		case Mode::GREEN: {
			auto g = csl::vec_y( color );
			color = csl::vec4( g, g, g, csl::scalar_one() );
			break;
		}

		case Mode::BLUE: {
			auto b = csl::vec_z( color );
			color = csl::vec4( b, b, b, csl::scalar_one() );
			break;
		}

		case Mode::ALPHA: {
			auto a = csl::vec_w( color );
			color = csl::vec4( a, a, a, csl::scalar_one() );
			break;
		}
			
		default:
			break;
	}

	master->setColor( vec3( color ) );
	master->setAlpha( alpha( color ) );

	buildFragmentShader( graph );
}

