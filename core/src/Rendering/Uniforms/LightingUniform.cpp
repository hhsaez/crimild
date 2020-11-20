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
*     * Neither the name of the copyright holder nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Rendering/Uniforms/LightingUniform.hpp"

#include "Rendering/ShadowMap.hpp"
#include "SceneGraph/Light.hpp"

using namespace crimild;

LightingUniform::LightingUniform( const Array< Light * > &lights ) noexcept
    : UniformBuffer( Lighting {} )
{
    lights.each(
        [ & ]( auto light ) {
            switch ( light->getType() ) {
                case Light::Type::AMBIENT:
                    m_ambientLights.add( light );
                    break;

                case Light::Type::POINT:
                    m_pointLights.add( light );
                    break;

                case Light::Type::DIRECTIONAL:
                    m_directionalLights.add( light );
                    break;

                case Light::Type::SPOT:
                    m_spotlights.add( light );
                    break;

                default:
                    break;
            }
        } );
}

void LightingUniform::onPreRender( void ) noexcept
{
    auto &lighting = getValue< Lighting >();

    auto copyLightInfo = []( auto &lights, auto &dst, auto max ) {
        auto count = Numerici::min( max, lights.size() );
        for ( auto i = 0l; i < count; ++i ) {
            auto &light = lights[ i ];
            dst[ i ].type = static_cast< UInt32 >( light->getType() );
            dst[ i ].position = light->getPosition().xyzw();
            dst[ i ].direction = light->getDirection().xyzw();
            dst[ i ].color = light->getColor().xyzw();
            dst[ i ].attenuation = light->getAttenuation().xyzw();
            dst[ i ].ambient = light->getAmbient().xyzw();
            dst[ i ].cutoff = Vector4f(
                Numericf::cos( light->getInnerCutoff() ),
                Numericf::cos( light->getOuterCutoff() ),
                0.0f,
                0.0f );
            dst[ i ].castShadows = light->castShadows();
            if ( light->castShadows() ) {
                dst[ i ].shadowBias = light->getShadowMap()->getBias();
                dst[ i ].cascadeSplits = light->getShadowMap()->getCascadeSplits();
                for ( auto split = 0; split < 4; ++split ) {
                    dst[ i ].lightSpaceMatrix[ split ] = light->getShadowMap()->getLightProjectionMatrix( split );
                }
                dst[ i ].viewport = light->getShadowMap()->getViewport();
            }
        }
        return count;
    };

    lighting.ambientLightCount = copyLightInfo( m_ambientLights, lighting.ambientLights, MAX_AMBIENT_LIGHTS );
    lighting.directionalLightCount = copyLightInfo( m_directionalLights, lighting.directionalLights, MAX_DIRECTIONAL_LIGHTS );
    lighting.pointLightCount = copyLightInfo( m_pointLights, lighting.pointLights, MAX_POINT_LIGHTS );
    lighting.spotlightCount = copyLightInfo( m_spotlights, lighting.spotlights, MAX_SPOTLIGHTS );
}
