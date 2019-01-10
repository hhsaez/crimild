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

#include "SkyboxShaderProgram.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/CSL.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/Texture.hpp"

using namespace crimild;
using namespace crimild::shadergraph;
using namespace crimild::shadergraph::csl;

SkyboxShaderProgram::SkyboxShaderProgram( void )
{
	attachUniforms({
		_viewMatrix = crimild::alloc< Matrix4fUniform >( "uVMatrix", Matrix4f::IDENTITY ),
		_projMatrix = crimild::alloc< Matrix4fUniform >( "uPMatrix", Matrix4f::IDENTITY ),
		_diffuse = crimild::alloc< RGBAColorfUniform >( "uDiffuse", RGBAColorf::ONE ),
		_cubeMap = crimild::alloc< TextureUniform >( "uCubeMap", nullptr ),
	});
	
	createVertexShader();
	createFragmentShader();
}

SkyboxShaderProgram::~SkyboxShaderProgram( void )
{

}

void SkyboxShaderProgram::createVertexShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto pMatrix = mat4_uniform( _projMatrix );
	auto vMatrix = mat4( mat3( mat4_uniform( _viewMatrix ) ) );
	auto p = modelPosition();
	auto uv = p;
	p = mult( pMatrix, vMatrix, vec4( p, scalar_one() ) );
	// Make sure the skybox always has the maximum depth value (1)
	p = vec4(
		vec_x( p ),
		vec_y( p ),
		vec_w( p ),
		vec_w( p )
	);

	csl::vertexOutput( "vTextureCoord", uv );
	csl::vertexPosition( p );

	auto src = graph->build();
	auto shader = crimild::alloc< VertexShader >( src );
	setVertexShader( shader );
}

void SkyboxShaderProgram::createFragmentShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto uv = vec3_in( "vTextureCoord" );
	auto texture = textureCube_uniform( _cubeMap );
	auto diffuse = vec4_uniform( _diffuse );

	auto color = mult(
		textureColor( texture, uv ),
		diffuse
	);
	fragColor( color );

	auto src = graph->build();
	auto shader = crimild::alloc< FragmentShader >( src );
	setFragmentShader( shader );
}

