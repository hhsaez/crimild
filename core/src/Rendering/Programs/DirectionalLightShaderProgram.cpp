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

#include "DirectionalLightShaderProgram.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/CSL.hpp"
#include "Rendering/Texture.hpp"
#include "SceneGraph/Light.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

DirectionalLightShaderProgram::DirectionalLightShaderProgram( void )
	: _lightColor( crimild::alloc< RGBAColorfUniform >( "uLightColor", RGBAColorf::ONE ) ),
	  _lightDirection( crimild::alloc< Vector3fUniform >( "uLightDirection", Vector3f::UNIT_Z ) ),
	  _normals( crimild::alloc< TextureUniform >( "uNormalTexture", Texture::ZERO ) )
{
	createVertexShader();
	createFragmentShader();
	
	attachUniform( _lightColor );
	attachUniform( _lightDirection );
	attachUniform( _normals );
}

DirectionalLightShaderProgram::~DirectionalLightShaderProgram( void )
{

}

void DirectionalLightShaderProgram::createVertexShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto p = csl::screenPosition();
	auto uv = csl::modelTextureCoords();

	csl::vertexOutput( "vTextureCoord", uv );
	csl::vertexPosition( p );

	auto src = graph->build();
	auto shader = crimild::alloc< VertexShader >( src );
	setVertexShader( shader );
}

void DirectionalLightShaderProgram::createFragmentShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto uv = csl::vec2_in( "vTextureCoord" );
	auto lc = csl::vec4_uniform( _lightColor );
	auto ld = csl::vec3_uniform( _lightDirection );
	auto ns = csl::texture2D_uniform( _normals );
	auto N = csl::vec3( csl::textureColor( ns, uv ) );
	auto diff = csl::max( csl::scalar( 0 ), csl::dot( N, ld ) );
	auto c = csl::mult( diff, lc );
	csl::fragColor( csl::vec4( csl::vec3( c ), csl::scalar( 1 ) ) );
	
	auto src = graph->build();
	auto shader = crimild::alloc< FragmentShader >( src );
	setFragmentShader( shader );
}

