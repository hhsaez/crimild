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

#include "UnlitShaderProgram.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/CSL.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

UnlitShaderProgram::UnlitShaderProgram( void )
{
	createVertexShader();
	createFragmentShader();

	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM, "uPMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM, "uVMatrix" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM, "uMMatrix" );

    registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_DIFFUSE_UNIFORM, "uMatDiffuse" );
    
    registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_USE_COLOR_MAP_UNIFORM, "uUseColorMap" );
	registerStandardLocation( ShaderLocation::Type::UNIFORM, ShaderProgram::StandardLocation::MATERIAL_COLOR_MAP_UNIFORM, "uColorMap" );
}

UnlitShaderProgram::~UnlitShaderProgram( void )
{

}

void UnlitShaderProgram::createVertexShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto p = csl::projectedPosition();
	auto uv = csl::modelTextureCoords();

	csl::vertexOutput( "vTextureCoord", uv );
	csl::vertexPosition( p );

	auto src = graph->build();
	auto shader = crimild::alloc< VertexShader >( src );
	setVertexShader( shader );
}

void UnlitShaderProgram::createFragmentShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto uv = csl::vec2_in( "vTextureCoord" );
	auto texture = csl::texture2D_uniform( "uColorMap" );
	auto diffuse = csl::vec4_uniform( "uMatDiffuse" );
	auto color = csl::mult(
		csl::textureColor( texture, uv ),
		diffuse
	);
	csl::fragColor( color );

	auto src = graph->build();
	auto shader = crimild::alloc< FragmentShader >( src );
	setFragmentShader( shader );
}
