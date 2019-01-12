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

#include "ViewSpaceNormalShaderProgram.hpp"

#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/CSL.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

ViewSpaceNormalShaderProgram::ViewSpaceNormalShaderProgram( void )
	: _roughness( crimild::alloc< FloatUniform >( "uRoughness", 32.0f ) ),
	  _pMatrix( crimild::alloc< Matrix4fUniform >( "uPMatrix", Matrix4f::IDENTITY ) ),
	  _vMatrix( crimild::alloc< Matrix4fUniform >( "uVMatrix", Matrix4f::IDENTITY ) ),
	  _mMatrix( crimild::alloc< Matrix4fUniform >( "uMMatrix", Matrix4f::IDENTITY ) ),
	  _nMatrix( crimild::alloc< Matrix3fUniform >( "uNMatrix", Matrix3f::IDENTITY ) )
{
	createVertexShader();
	createFragmentShader();

	attachUniform( _roughness );
	attachUniform( _pMatrix );
	attachUniform( _vMatrix );
	attachUniform( _mMatrix );
	attachUniform( _nMatrix );
}

ViewSpaceNormalShaderProgram::~ViewSpaceNormalShaderProgram( void )
{
	
}

void ViewSpaceNormalShaderProgram::createVertexShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto P = csl::clipPosition();
	auto N = csl::viewNormal();
	
	csl::vertexOutput( "vViewNormal", N );
	csl::vertexPosition( P );
	
	auto src = graph->build();
	auto shader = crimild::alloc< VertexShader >( src );
	setVertexShader( shader );
}

void ViewSpaceNormalShaderProgram::createFragmentShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();
	
	auto N = csl::vec3_in( "vViewNormal" );
	auto R = csl::scalar_uniform( _roughness );
	csl::fragColor( csl::vec4( N, R ) );
	
	auto src = graph->build();
	auto shader = crimild::alloc< FragmentShader >( src );
	setFragmentShader( shader );
}

