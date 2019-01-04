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

#include "PhongSpecularShaderProgram.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/CSL.hpp"
#include "Rendering/Texture.hpp"
#include "SceneGraph/Light.hpp"

using namespace crimild;
using namespace crimild::shadergraph;
using namespace crimild::shadergraph::csl;	

PhongSpecularShaderProgram::PhongSpecularShaderProgram( void )
	: _lightColor( crimild::alloc< RGBAColorfUniform >( "uLightColor", RGBAColorf::ONE ) ),
	  _lightDirection( crimild::alloc< Vector3fUniform >( "uLightDirection", Vector3f::UNIT_Z ) ),
	  _invProjMatrix( crimild::alloc< Matrix4fUniform >( "uInvPMatrix", Matrix4f(
		  1.0f, 0.0f, 0.0f, 0.0f,
		  0.0f, 1.0f, 0.0f, 0.0f,
		  0.0f, 0.0f, 1.0f, 0.0f,
		  0.0f, 0.0f, 0.0f, 1.0f
	  ))),
	  _normalTexture( crimild::alloc< TextureUniform >( "uNormalTexture", Texture::ZERO ) ),
	  _depthTexture( crimild::alloc< TextureUniform >( "uDepthTexture", Texture::ZERO ) )
{
	createVertexShader();
	createFragmentShader();
	
	attachUniform( _lightColor );
	attachUniform( _lightDirection );
	attachUniform( _invProjMatrix );
	attachUniform( _normalTexture );
	attachUniform( _depthTexture );
}

PhongSpecularShaderProgram::~PhongSpecularShaderProgram( void )
{

}

void PhongSpecularShaderProgram::createVertexShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto p = screenPosition();
	auto uv = modelTextureCoords();

	auto depth = texture2D_uniform( _depthTexture );
	auto invPMatrix = mat4_uniform( _invProjMatrix );

	auto P = vec4(
		vec_x( uv ),
		vec_y( uv ),
		vec_x( textureColor( depth, uv ) ),
		scalar_one()
	);
	// clip-space [-1, 1]
	P = sub( mult( scalar_two(), P ), scalar_one() );
	// view-space
    P = mult( invPMatrix, P );
	
	vertexOutput( "vFragPos", vec3( neg( P ) ) );
	vertexOutput( "vTextureCoord", uv );
	vertexPosition( p );

	auto src = graph->build();
	auto shader = crimild::alloc< VertexShader >( src );
	setVertexShader( shader );
}

void PhongSpecularShaderProgram::createFragmentShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto fragPos = vec3_in( "vFragPos" );
	auto uv = vec2_in( "vTextureCoord" );
	
	auto lc = vec4_uniform( _lightColor );
	auto ld = vec3_uniform( _lightDirection );
	auto ns = texture2D_uniform( _normalTexture );

	auto NR = textureColor( ns, uv );
	auto N = normalize( vec3( NR ) );
	auto viewDir = normalize( fragPos );
	auto reflectDir = reflect( neg( ld ), N );
	
	auto specStrength = scalar( 1.0 );
	auto shininess = vec_w( NR );
	auto spec = pow( max( dot( viewDir, reflectDir ), scalar_zero() ), shininess );
	auto cSpecular = vec3( mult( specStrength, spec, lc ) );
	
	fragColor( vec4( cSpecular, scalar_one() ) );
	
	auto src = graph->build();
	auto shader = crimild::alloc< FragmentShader >( src );
	setFragmentShader( shader );
}

