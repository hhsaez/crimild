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

#include "PhongDeferredLightingShaderProgram.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/CSL.hpp"
#include "Rendering/Texture.hpp"

using namespace crimild;
using namespace crimild::shadergraph;
using namespace crimild::shadergraph::csl;

PhongDeferredLightingShaderProgram::PhongDeferredLightingShaderProgram( void )
	: _modelMatrix( crimild::alloc< Matrix4fUniform >( "uMMatrix", Matrix4f::IDENTITY ) ),
	  _viewMatrix( crimild::alloc< Matrix4fUniform >( "uVMatrix", Matrix4f::IDENTITY ) ),
	  _projMatrix( crimild::alloc< Matrix4fUniform >( "uPMatrix", Matrix4f::IDENTITY ) ),
	  _lightAmbientTexture( crimild::alloc< TextureUniform >( "uLightAmbientTexture", Texture::ZERO ) ),
	  _lightDiffuseTexture( crimild::alloc< TextureUniform >( "uLightDiffuseTexture", Texture::ZERO ) ),
	  _lightSpecularTexture( crimild::alloc< TextureUniform >( "uLightSpecularTexture", Texture::ZERO ) ),
	  _materialAmbient( crimild::alloc< RGBAColorfUniform >( "uMaterialAmbient", RGBAColorf::ZERO ) ),
	  _materialDiffuse( crimild::alloc< RGBAColorfUniform >( "uMaterialDiffuse", RGBAColorf::ZERO ) ),
	  _materialSpecular( crimild::alloc< RGBAColorfUniform >( "uMaterialSpecular", RGBAColorf::ZERO ) ),
	  _screenSize( crimild::alloc< Vector2fUniform >( "uScreenSize", Vector2f::ONE ) )
{
	createVertexShader();
	createFragmentShader();

	attachUniforms({
		_modelMatrix,
		_viewMatrix,
		_projMatrix,
		_lightAmbientTexture,
		_lightDiffuseTexture,
		_lightSpecularTexture,
		_materialAmbient,
		_materialDiffuse,
		_materialSpecular,
		_screenSize,
	});
}

PhongDeferredLightingShaderProgram::~PhongDeferredLightingShaderProgram( void )
{

}

void PhongDeferredLightingShaderProgram::createVertexShader( void )
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

void PhongDeferredLightingShaderProgram::createFragmentShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto uv = csl::vec2_in( "vTextureCoord" );
	auto fragCoord = csl::vec2( csl::fragCoord() );
	auto screenSize = vec2_uniform( _screenSize );
	fragCoord = div( fragCoord, screenSize );
	
	auto lA = csl::textureColor( csl::texture2D_uniform( _lightAmbientTexture ), fragCoord );
	auto lD = csl::textureColor( csl::texture2D_uniform( _lightDiffuseTexture ), fragCoord );
	auto lS = csl::textureColor( csl::texture2D_uniform( _lightSpecularTexture ), fragCoord );
	auto mA = csl::vec4_uniform( _materialAmbient );
	auto mD = csl::vec4_uniform( _materialDiffuse );
	auto mS = csl::vec4_uniform( _materialSpecular );

	auto color = add(
		mult( lA, mA ),
		mult( lD, mD ),
		mult( lS, mS )
	);
	
	csl::fragColor( color );

	auto src = graph->build();
	auto shader = crimild::alloc< FragmentShader >( src );
	setFragmentShader( shader );
}

