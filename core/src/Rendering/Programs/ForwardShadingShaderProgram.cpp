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
#include "Rendering/ShaderGraph/Variable.hpp"
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
			light.position = crimild::alloc< Vector4fUniform >( lightName + "_position", Vector4f::ZERO ),
			light.direction = crimild::alloc< Vector4fUniform >( lightName + "_direction", Vector4f::ZERO ),
			light.attenuation = crimild::alloc< Vector4fUniform >( lightName + "_attenuation", Vector4f::ZERO ),
			light.innerCutOff = crimild::alloc< FloatUniform >( lightName + "_innerCutOFF", 0.0f ),
			light.outerCutOff = crimild::alloc< FloatUniform >( lightName + "_outerCutOFF", 0.0f ),
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
			case Light::Type::AMBIENT: {
				uniforms.ambientColor->setValue( light->getAmbient() );
				uniforms.diffuseColor->setValue( RGBAColorf::ZERO );
				uniforms.attenuation->setValue( RGBAColorf::ZERO );
				break;
			}

			case Light::Type::DIRECTIONAL: {
				uniforms.ambientColor->setValue( RGBAColorf::ZERO );
				uniforms.diffuseColor->setValue( light->getColor() );
				uniforms.position->setValue( Vector4f::ZERO );
				auto d = light->getDirection();
				uniforms.direction->setValue( Vector4f( -d.x(), -d.y(), -d.z(), 0.0f ) );
				uniforms.attenuation->setValue( Vector4f( 1.0f, 0.0f, 0.0f, 0.0f ) );
				break;
			}

			case Light::Type::POINT: {
				uniforms.ambientColor->setValue( RGBAColorf::ZERO );
				uniforms.diffuseColor->setValue( light->getColor() );
				auto p = light->getWorld().getTranslate();
				uniforms.position->setValue( Vector4f( p.x(), p.y(), p.z(), 1.0f ) );
				uniforms.direction->setValue( Vector4f::ZERO );
				auto a = light->getAttenuation();
				uniforms.attenuation->setValue( Vector4f( a.x(), a.y(), a.z(), 1.0f ) );
				break;
			}

			case Light::Type::SPOT: {
				uniforms.ambientColor->setValue( RGBAColorf::ZERO );
				uniforms.diffuseColor->setValue( light->getColor() );
				auto p = light->getWorld().getTranslate();
				uniforms.position->setValue( Vector4f( p.x(), p.y(), p.z(), 1.0f ) );
				auto d = light->getDirection();
				uniforms.direction->setValue( Vector4f( d.x(), d.y(), d.z(), 1.0f ) );
				auto a = light->getAttenuation();
				uniforms.attenuation->setValue( Vector4f( a.x(), a.y(), a.z(), 1.0f ) );
				uniforms.innerCutOff->setValue( Numericf::cos( light->getInnerCutoff() ) );
				uniforms.outerCutOff->setValue( Numericf::cos( light->getOuterCutoff() ) );
				break;
			}

			default: {
				uniforms.ambientColor->setValue( RGBAColorf::ZERO );
				uniforms.diffuseColor->setValue( RGBAColorf::ZERO );
				uniforms.attenuation->setValue( RGBAColorf::ZERO );
				break;
			}
		}
	}
}

void ForwardShadingShaderProgram::createVertexShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto P = projectedPosition();
	auto WP = worldPosition();
	auto WE = worldEyeVector();
	auto WN = worldNormal();
	auto UV = modelTextureCoords();

	vertexOutput( "vTextureCoord", UV );
	vertexOutput( "vWorldNormal", WN );
	vertexOutput( "vWorldEye", WE );
	vertexOutput( "vWorldPosition", vec3( WP ) );
	vertexPosition( P );

	auto src = graph->build();
	auto shader = crimild::alloc< VertexShader >( src );
	setVertexShader( shader );
}

void ForwardShadingShaderProgram::createFragmentShader( void )
{
	auto graph = Renderer::getInstance()->createShaderGraph();

	auto uv = vec2_in( "vTextureCoord" );
	auto N = normalize( vec3_in( "vWorldNormal" ) );
	auto E = normalize( vec3_in( "vWorldEye" ) );
	auto P = vec3_in( "vWorldPosition" );

	auto matAmbient = vec4_uniform( _matAmbient );
	auto matDiffuse = vec4_uniform( _matDiffuse );
	auto matSpecular = vec4_uniform( _matSpecular );
	auto matShininess = scalar_uniform( _matShininess );

	auto accumAmbient = vec3( scalar_zero() );
	auto accumDiffuse = vec3( scalar_zero() );
	auto accumSpecular = vec3( scalar_zero() );

	_lights.each( [ this, &accumAmbient, &accumDiffuse, &accumSpecular, N, E, P, matShininess ]( LightUniforms &light, crimild::Size lightIndex ) {
		auto lAmbient = vec3( vec4_uniform( light.ambientColor ) );
		auto lDiffuse = vec3( vec4_uniform( light.diffuseColor ) );
		auto lDirectionXYZW = vec4_uniform( light.direction );
		auto lDirection = vec3( lDirectionXYZW );
		auto lHasDirection = vec_w( lDirectionXYZW );		
		auto lPositionXYZW = vec4_uniform( light.position );
		auto lPosition = vec3( lPositionXYZW );
		auto lHasPosition = vec_w( lPositionXYZW );
		auto lAttenuation = vec4_uniform( light.attenuation );
		auto lAttConstant = vec_x( lAttenuation );
		auto lAttLinear = vec_y( lAttenuation );
		auto lAttQuadratic = vec_y( lAttenuation );
		auto lAttEnabled = vec_w( lAttenuation );
		auto lInnerCutOff = scalar_uniform( light.innerCutOff );
		auto lOuterCutOff = scalar_uniform( light.outerCutOff );
		
		auto lVector = add(
			mult( sub( scalar_one(), lHasPosition ), lDirection ),
			mult( lHasPosition, sub( lPosition, P ) )
		);
		auto lUnitVector = normalize( lVector );
		auto lDistance = length( lVector );
		auto R = reflect( neg( lUnitVector ), N );

		auto ambient = lAmbient;

		auto diffuse = mult( 
			max( scalar_zero(), dot( N, lUnitVector ) ),
			lDiffuse
		);

		auto specular = mult(
			pow( max( dot( E, R ), scalar_zero() ), matShininess ),
			lDiffuse
		);

		// spotlight
		auto isSpotlight = mult( lHasPosition, lHasDirection );
		auto theta = dot( lUnitVector, normalize( neg( lDirection ) ) );
		auto epsilon = sub( lInnerCutOff, lOuterCutOff );
		auto intensity = clamp( div( sub( theta, lOuterCutOff ), epsilon ), scalar_zero(), scalar_one() );
		intensity = add(
			mult( isSpotlight, intensity ),
			sub( scalar_one(), isSpotlight )
		);
		diffuse = mult( diffuse, intensity );
		specular = mult( specular, intensity );

		auto attenuation = div(
			scalar_one(),
			add(
				lAttConstant,
				mult( lAttLinear, lDistance ),
				mult( lAttQuadratic, mult( lDistance, lDistance ) )
			)
		);

		attenuation = add(
			sub( scalar_one(), lAttEnabled ),
			mult( lAttEnabled, attenuation )
		);

		ambient = mult( ambient, attenuation );
		diffuse = mult( diffuse, attenuation );
		specular = mult( specular, attenuation );

		// accumulate
		accumAmbient = add( accumAmbient, ambient );
		accumDiffuse = add( accumDiffuse, diffuse );
		accumSpecular = add( accumSpecular, specular );
	});

	accumAmbient = mult(
		accumAmbient,
		vec3( matAmbient )
	);

	accumDiffuse = mult(
		accumDiffuse,
		vec3( matDiffuse ),
		vec3( textureColor( texture2D_uniform( _matDiffuseMap ), uv ) )
	);

	accumSpecular = mult(
		accumSpecular,
		vec3( matSpecular ),
		vec3( textureColor( texture2D_uniform( _matSpecularMap ), uv ) )
	);

	auto color = add(
		accumAmbient,
		accumDiffuse,
		accumSpecular
	);

	csl::fragColor( vec4( color, scalar_one() ) );

	auto src = graph->build();
	auto shader = crimild::alloc< FragmentShader >( src );
	setFragmentShader( shader );
}

