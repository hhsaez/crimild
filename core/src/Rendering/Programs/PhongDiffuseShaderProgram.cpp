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

#include "PhongDiffuseShaderProgram.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/CSL.hpp"
#include "Rendering/Texture.hpp"
#include "SceneGraph/Light.hpp"

using namespace crimild;
using namespace crimild::shadergraph;

PhongDiffuseShaderProgram::PhongDiffuseShaderProgram( void )
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

PhongDiffuseShaderProgram::~PhongDiffuseShaderProgram( void )
{

}

void PhongDiffuseShaderProgram::createVertexShader( void )
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

void PhongDiffuseShaderProgram::createFragmentShader( void )
{
	using namespace crimild::shadergraph::csl;
	
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto uv = vec2_in( "vTextureCoord" );
	
	auto lc = vec4_uniform( _lightColor );
	auto ld = vec3_uniform( _lightDirection );
	auto ns = texture2D_uniform( _normals );
	
	auto N = normalize( vec3( textureColor( ns, uv ) ) );
	
	auto diff = max( scalar_zero(), dot( N, ld ) );
	auto cDiff = vec3( mult( diff, lc ) );

	fragColor( vec4( cDiff, scalar_one() ) );
	
	auto src = graph->build();
	auto shader = crimild::alloc< FragmentShader >( src );
	setFragmentShader( shader );
}

