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

#include "ForwardShadingShaderProgram.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/CSL.hpp"
#include "SceneGraph/Light.hpp"

using namespace crimild;
using namespace crimild::shadergraph;
using namespace crimild::shadergraph::csl;

ForwardShadingShaderProgram::ForwardShadingShaderProgram( crimild::Size maxLights )
	: _lights( maxLights )
{
	attachUniforms({
		_modelMatrix = crimild::alloc< Matrix4fUniform >( "uMMatrix", Matrix4f::IDENTITY ),
		_normalMatrix = crimild::alloc< Matrix3fUniform >( "uNMatrix", Matrix3f::IDENTITY ),
		_viewMatrix = crimild::alloc< Matrix4fUniform >( "uVMatrix", Matrix4f::IDENTITY ),
		_projMatrix = crimild::alloc< Matrix4fUniform >( "uPMatrix", Matrix4f::IDENTITY ),
		_matAmbient = crimild::alloc< RGBAColorfUniform >( "uMaterialAmbient", RGBAColorf::ONE ),
		_matDiffuse = crimild::alloc< RGBAColorfUniform >( "uMaterialDiffuse", RGBAColorf::ONE ),
		_matDiffuseMap = crimild::alloc< TextureUniform >( "uMaterialTexture", Texture::ONE ),
		_matSpecular = crimild::alloc< RGBAColorfUniform >( "uMaterialSpecular", RGBAColorf::ONE ),
		_matSpecularMap = crimild::alloc< TextureUniform >( "uMaterialSpecularMap", Texture::ONE ),
		_matShininess = crimild::alloc< FloatUniform >( "uMaterialShininess", 16.0f ),
	});

	for ( crimild::Size i = 0; i < maxLights; i++ ) {
		std::stringstream ss;
		ss << "uLight_" << i;
		auto lightName = ss.str();

		auto &light = _lights[ i ];
		attachUniforms({
			light.ambientColor = crimild::alloc< RGBAColorfUniform >( lightName + "_ambient", RGBAColorf::ZERO ),
			light.diffuseColor = crimild::alloc< RGBAColorfUniform >( lightName + "_diffuse", RGBAColorf::ZERO ),
			light.direction = crimild::alloc< Vector3fUniform >( lightName + "_direction", Vector3f::UNIT_Z ),
		});
	}
	
	createVertexShader();
	createFragmentShader();
}

ForwardShadingShaderProgram::~ForwardShadingShaderProgram( void )
{

}

void ForwardShadingShaderProgram::bindMaterial( Material *material )
{
	if ( material != nullptr ) {
		_matAmbient->setValue( material->getAmbient() );
		_matDiffuse->setValue( material->getDiffuse() );
		_matDiffuseMap->setValue( material->getColorMap() != nullptr ? material->getColorMap() : crimild::get_ptr( Texture::ONE ) );
		_matSpecular->setValue( material->getSpecular() );
		_matSpecularMap->setValue( material->getSpecularMap() != nullptr ? material->getSpecularMap() : crimild::get_ptr( Texture::ONE ) );
		_matShininess->setValue( material->getShininess() );		
	}
}

void ForwardShadingShaderProgram::bindLight( Light *light, crimild::Size index )
{
	if ( light != nullptr && index < _lights.size() ) {
		auto &uniforms = _lights[ index ];
		switch ( light->getType() ) {
			case Light::Type::AMBIENT:
				uniforms.ambientColor->setValue( light->getAmbient() );
				uniforms.diffuseColor->setValue( RGBAColorf::ZERO );
				break;

			case Light::Type::DIRECTIONAL:
				uniforms.ambientColor->setValue( RGBAColorf::ZERO );
				uniforms.diffuseColor->setValue( light->getColor() );
				uniforms.direction->setValue( -light->getDirection() );
				break;

			default:
				uniforms.ambientColor->setValue( RGBAColorf::ZERO );
				uniforms.diffuseColor->setValue( RGBAColorf::ZERO );
				break;
		}
	}
}

void ForwardShadingShaderProgram::createVertexShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto p = projectedPosition();
	auto v = viewVector();
	auto uv = modelTextureCoords();
	auto n = viewNormal();

	vertexOutput( "vTextureCoord", uv );
	vertexOutput( "vViewNormal", n );
	vertexOutput( "vViewVector", v );
	vertexPosition( p );

	auto src = graph->build();
	auto shader = crimild::alloc< VertexShader >( src );
	setVertexShader( shader );
}

void ForwardShadingShaderProgram::createFragmentShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto uv = vec2_in( "vTextureCoord" );
	auto n = normalize( vec3_in( "vViewNormal" ) );
	auto v = normalize( vec3_in( "vViewVector" ) );
	auto vMatrix = mat3( mat4_uniform( "uVMatrix" ) );

	auto matDiffuse = vec4_uniform( _matDiffuse );
	auto matAmbient = vec4_uniform( _matAmbient );
	auto matSpecular = vec4_uniform( _matSpecular );
	auto matShininess = scalar_uniform( _matShininess );

	auto ambient = vec4( scalar_zero() );
	auto diffuse = vec4( scalar_zero() );
	auto specular = vec4( scalar_zero() );

	_lights.each( [ this, &ambient, &diffuse, &specular, n, v, matShininess, vMatrix ]( LightUniforms &light ) {
		auto lAmbient = vec4_uniform( light.ambientColor );
		auto lDiffuse = vec4_uniform( light.diffuseColor );
		auto lDirection = normalize( mult( vMatrix, vec3_uniform( light.direction ) ) );

		ambient = add( ambient, lAmbient );

		diffuse = add(
			diffuse,
			mult( 
				max( scalar_zero(), dot( n, lDirection ) ),
				lDiffuse
			)
		);

		specular = add(
			specular,
			mult(
				pow( max( dot( v, reflect( neg( lDirection ), n ) ), scalar_zero() ), matShininess ),
				lDiffuse
			)
		);
	});

	ambient = mult(
		ambient,
		matAmbient
	);

	diffuse = mult(
		diffuse,
		matDiffuse,
		textureColor( texture2D_uniform( _matDiffuseMap ), uv )
	);

	specular = mult(
		specular,
		matSpecular,
		textureColor( texture2D_uniform( _matSpecularMap ), uv )
	);

	auto color = add(
		ambient,
		diffuse,
		specular
	);

	csl::fragColor( vec4( vec3( color ), scalar_one() ) );

	auto src = graph->build();
	auto shader = crimild::alloc< FragmentShader >( src );
	setFragmentShader( shader );
}

