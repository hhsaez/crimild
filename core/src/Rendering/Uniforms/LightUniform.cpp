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

#include "Rendering/Uniforms/LightUniform.hpp"

#include "Mathematics/swizzle.hpp"
#include "Rendering/ShadowMap.hpp"
#include "SceneGraph/Light.hpp"

using namespace crimild;

LightUniform::LightUniform( Light *light ) noexcept
    : UniformBuffer( LightProps {} ),
      m_light( light )
{
    // no-op
}

void LightUniform::onPreRender( void ) noexcept
{
    auto &props = getValue< LightProps >();

    props.type = static_cast< UInt32 >( m_light->getType() );
    props.position = vector4( m_light->getPosition(), Real( 1 ) );
    props.direction = vector4( m_light->getDirection(), Real( 0 ) );
    props.color = m_light->getColor();
    props.attenuation = vector4( m_light->getAttenuation(), Real( 0 ) );
    props.ambient = m_light->getAmbient();
    props.cutoff = Vector4f {
        Numericf::cos( m_light->getInnerCutoff() ),
        Numericf::cos( m_light->getOuterCutoff() ),
        0.0f,
        0.0f,
    };
    props.castShadows = m_light->castShadows() ? 1 : 0;
    if ( m_light->castShadows() ) {
        props.shadowBias = m_light->getShadowMap()->getBias();
        props.cascadeSplits = m_light->getShadowMap()->getCascadeSplits();
        for ( auto split = 0; split < 4; ++split ) {
            props.lightSpaceMatrix[ split ] = m_light->getShadowMap()->getLightProjectionMatrix( split );
        }
        props.viewport = m_light->getShadowMap()->getViewport();
    }
    props.energy = m_light->getEnergy();
    props.radius = m_light->getRadius();
}
