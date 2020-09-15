/*
 * Copyright (c) 2002 - present, H. Hernan Saez
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

#include "LightCatalog.hpp"

#include "Rendering/OpenGLUtils.hpp"

#include <Foundation/Log.hpp>
#include <Rendering/Renderer.hpp>
#include <Rendering/ShaderLocation.hpp>
#include <Rendering/ShadowMap.hpp>
#include <SceneGraph/Light.hpp>

using namespace crimild;
using namespace crimild::shadergraph;
using namespace crimild::opengl;

LightCatalog::LightCatalog( void )
{
}

LightCatalog::~LightCatalog( void )
{
    if ( _ambientLightBlockId > 0 ) {
        glDeleteBuffers( 1, &_ambientLightBlockId );
        _ambientLightBlockId = 0;
    }

    if ( _directionalLightBlockId > 0 ) {
        glDeleteBuffers( 1, &_directionalLightBlockId );
        _directionalLightBlockId = 0;
    }

    if ( _pointLightBlockId > 0 ) {
        glDeleteBuffers( 1, &_pointLightBlockId );
        _pointLightBlockId = 0;
    }

    if ( _spotLightBlockId > 0 ) {
        glDeleteBuffers( 1, &_spotLightBlockId );
        _spotLightBlockId = 0;
    }
}

void LightCatalog::configure( void )
{
    Catalog< Light >::configure();

    /*

	auto renderer = Renderer::getInstance();

    // initialize ambient block
	auto maxAmbientLights = renderer->getMaxLights( Light::Type::AMBIENT );
	_usedAmbientLightIds.resize( maxAmbientLights );
	_ambientLights.resize( maxAmbientLights );

	glGenBuffers( 1, &_ambientLightBlockId );
	glBindBuffer( GL_UNIFORM_BUFFER, _ambientLightBlockId );
	glBufferData( GL_UNIFORM_BUFFER, sizeof( AmbientLightData ) * maxAmbientLights, nullptr, GL_STATIC_DRAW );
	glBindBuffer( GL_UNIFORM_BUFFER, 0 );

    glBindBufferBase( GL_UNIFORM_BUFFER, blocks::AMBIENT_LIGHT_UNIFORM_BLOCK_BINDING_POINT, _ambientLightBlockId );

    // initialize directional
	auto maxDirectionalLights = renderer->getMaxLights( Light::Type::DIRECTIONAL );
	_usedDirectionalLightIds.resize( maxDirectionalLights );
	_directionalLights.resize( maxDirectionalLights );

	glGenBuffers( 1, &_directionalLightBlockId );
	glBindBuffer( GL_UNIFORM_BUFFER, _directionalLightBlockId );
	glBufferData( GL_UNIFORM_BUFFER, sizeof( DirectionalLightData ) * maxDirectionalLights, nullptr, GL_STATIC_DRAW );
	glBindBuffer( GL_UNIFORM_BUFFER, 0 );

    glBindBufferBase( GL_UNIFORM_BUFFER, blocks::DIRECTIONAL_LIGHT_UNIFORM_BLOCK_BINDING_POINT, _directionalLightBlockId );

    // initialize point
    auto maxPointLights = renderer->getMaxLights( Light::Type::POINT );
    _usedPointLightIds.resize( maxPointLights );
    _pointLights.resize( maxPointLights );

    glGenBuffers( 1, &_pointLightBlockId );
    glBindBuffer( GL_UNIFORM_BUFFER, _pointLightBlockId );
    glBufferData( GL_UNIFORM_BUFFER, sizeof( PointLightData ) * maxPointLights, nullptr, GL_STATIC_DRAW );
    glBindBuffer( GL_UNIFORM_BUFFER, 0 );

    glBindBufferBase( GL_UNIFORM_BUFFER, blocks::POINT_LIGHT_UNIFORM_BLOCK_BINDING_POINT, _pointLightBlockId );

    // initialize spot
    auto maxSpotLights = renderer->getMaxLights( Light::Type::SPOT );
    _usedSpotLightIds.resize( maxSpotLights );
    _spotLights.resize( maxSpotLights );

    glGenBuffers( 1, &_spotLightBlockId );
    glBindBuffer( GL_UNIFORM_BUFFER, _spotLightBlockId );
    glBufferData( GL_UNIFORM_BUFFER, sizeof( SpotLightData ) * maxSpotLights, nullptr, GL_STATIC_DRAW );
    glBindBuffer( GL_UNIFORM_BUFFER, 0 );

    glBindBufferBase( GL_UNIFORM_BUFFER, blocks::SPOT_LIGHT_UNIFORM_BLOCK_BINDING_POINT, _spotLightBlockId );
    */
}

int LightCatalog::getNextResourceId( Light *light )
{
    CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

    switch ( light->getType() ) {
        case Light::Type::AMBIENT:
            for ( auto i = 0L; i < _usedAmbientLightIds.size(); i++ ) {
                if ( !_usedAmbientLightIds[ i ] ) {
                    _usedAmbientLightIds[ i ] = true;
                    return i;
                }
            }
            break;

        case Light::Type::DIRECTIONAL:
            for ( auto i = 0L; i < _usedDirectionalLightIds.size(); i++ ) {
                if ( !_usedDirectionalLightIds[ i ] ) {
                    _usedDirectionalLightIds[ i ] = true;
                    return i;
                }
            }
            break;

        case Light::Type::POINT:
            for ( auto i = 0L; i < _usedPointLightIds.size(); i++ ) {
                if ( !_usedPointLightIds[ i ] ) {
                    _usedPointLightIds[ i ] = true;
                    return i;
                }
            }
            break;

        case Light::Type::SPOT:
            for ( auto i = 0L; i < _usedSpotLightIds.size(); i++ ) {
                if ( !_usedSpotLightIds[ i ] ) {
                    _usedSpotLightIds[ i ] = true;
                    return i;
                }
            }
            break;

        default:
            break;
    }

    CRIMILD_LOG_WARNING( "No more lights avaiable" );

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;

    return 0;
}

void LightCatalog::bind( Light *light )
{
    assert( false );
    /*
	if ( light == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid light pointer" );
		return;
	}

	CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION;

	Catalog< Light >::bind( light );

	if ( light->getCatalog() == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Could not bind light" );
		return;
	}

    auto lightId = light->getCatalogId();

	if ( light->getType() == Light::Type::AMBIENT ) {

        auto &lightData = _ambientLights[ lightId ];
        lightData.ambient = light->getAmbient();

		glBindBuffer( GL_UNIFORM_BUFFER, _ambientLightBlockId );
		glBufferSubData( GL_UNIFORM_BUFFER, lightId * sizeof( AmbientLightData ), sizeof( AmbientLightData ), &_ambientLights[ lightId ] );
		glBindBuffer( GL_UNIFORM_BUFFER, 0 );
	}
    else if ( light->getType() == Light::Type::DIRECTIONAL ) {
        auto &lightData = _directionalLights[ lightId ];
        lightData.ambient = light->getAmbient();
        lightData.diffuse = light->getColor();
        lightData.direction = -light->getDirection().xyzw();
        lightData.castShadows = 0;
        if ( auto shadowMap = light->getShadowMap() ) {
            lightData.castShadows = 1;
            lightData.lightSpaceMatrix = shadowMap->getLightViewMatrix() * shadowMap->getLightProjectionMatrix();
            lightData.shadowMinMaxBias = Vector2f( shadowMap->getMinBias(), shadowMap->getMaxBias() );
            lightData.shadowMapViewport = shadowMap->getViewport();
        }

        glBindBuffer( GL_UNIFORM_BUFFER, _directionalLightBlockId );
        glBufferSubData( GL_UNIFORM_BUFFER, lightId * sizeof( DirectionalLightData ), sizeof( DirectionalLightData ), &_directionalLights[ lightId ] );
        glBindBuffer( GL_UNIFORM_BUFFER, 0 );
    }
    else if ( light->getType() == Light::Type::POINT ) {
        auto &lightData = _pointLights[ lightId ];
        lightData.ambient = light->getAmbient();
        lightData.diffuse = light->getColor();
        lightData.position = light->getWorld().getTranslate().xyzw();
		lightData.attenuation = light->getAttenuation().xyzw();

        glBindBuffer( GL_UNIFORM_BUFFER, _pointLightBlockId );
        glBufferSubData( GL_UNIFORM_BUFFER, lightId * sizeof( PointLightData ), sizeof( PointLightData ), &_pointLights[ lightId ] );
        glBindBuffer( GL_UNIFORM_BUFFER, 0 );
    }
    else if ( light->getType() == Light::Type::SPOT ) {
        auto &lightData = _spotLights[ lightId ];
        lightData.ambient = light->getAmbient();
        lightData.diffuse = light->getColor();
        lightData.direction = light->getDirection().xyzw();
        lightData.position = light->getWorld().getTranslate().xyzw();
		lightData.attenuation = light->getAttenuation().xyzw();
		lightData.innerCutOff = Numericf::cos( light->getInnerCutoff() );
		lightData.outerCutOff = Numericf::cos( light->getOuterCutoff() );
        lightData.castShadows = 0;
        if ( auto shadowMap = light->getShadowMap() ) {
            lightData.castShadows = 1;
            lightData.lightSpaceMatrix = shadowMap->getLightViewMatrix() * shadowMap->getLightProjectionMatrix();
            lightData.shadowMinMaxBias = Vector2f( shadowMap->getMinBias(), shadowMap->getMaxBias() );
            lightData.shadowMapViewport = shadowMap->getViewport();
        }

        glBindBuffer( GL_UNIFORM_BUFFER, _spotLightBlockId );
        glBufferSubData( GL_UNIFORM_BUFFER, lightId * sizeof( SpotLightData ), sizeof( SpotLightData ), &_spotLights[ lightId ] );
        glBindBuffer( GL_UNIFORM_BUFFER, 0 );
    }
    */

    CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION;
}
