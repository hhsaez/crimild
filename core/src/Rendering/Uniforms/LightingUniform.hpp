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

#ifndef CRIMILD_RENDERING_UNIFORMS_LIGHT_
#define CRIMILD_RENDERING_UNIFORMS_LIGHT_

#include "Foundation/Containers/Array.hpp"
#include "Mathematics/Matrix.hpp"
#include "Rendering/UniformBuffer.hpp"

namespace crimild {

    class Light;

    /**
       \todo This should be part of the Light class. That way, we can have different
       uniforms for different types of lights (point, directional, PBR, etc.)
     */
    class LightingUniform : public UniformBuffer {
    private:
        static constexpr const Size MAX_AMBIENT_LIGHTS = 1;
        static constexpr const Size MAX_DIRECTIONAL_LIGHTS = 2;
        static constexpr const Size MAX_POINT_LIGHTS = 10;
        static constexpr const Size MAX_SPOTLIGHTS = 4;

        struct LightProp {
            alignas( 16 ) UInt32 type;
            alignas( 16 ) Vector4f position;
            alignas( 16 ) Vector4f direction;
            alignas( 16 ) Vector4f ambient;
            alignas( 16 ) Vector4f color;
            alignas( 16 ) Vector4f attenuation;
            alignas( 16 ) Vector4f cutoff;
            alignas( 16 ) Bool castShadows;
            alignas( 16 ) Vector4f cascadeSplits;
            alignas( 16 ) Matrix4f lightSpaceMatrix[ 4 ];
            alignas( 16 ) Vector4f viewport;
        };

        struct Lighting {
            alignas( 16 ) LightProp ambientLights[ MAX_AMBIENT_LIGHTS ];
            alignas( 16 ) UInt32 ambientLightCount = 0;
            alignas( 16 ) LightProp directionalLights[ MAX_DIRECTIONAL_LIGHTS ];
            alignas( 16 ) UInt32 directionalLightCount = 0;
            alignas( 16 ) LightProp pointLights[ MAX_POINT_LIGHTS ];
            alignas( 16 ) UInt32 pointLightCount = 0;
            alignas( 16 ) LightProp spotlights[ MAX_SPOTLIGHTS ];
            alignas( 16 ) UInt32 spotlightCount = 0;
        };

    public:
        explicit LightingUniform( const Array< Light * > &lights ) noexcept;
        ~LightingUniform( void ) = default;

        void onPreRender( void ) noexcept override;

    private:
        Array< Light * > m_ambientLights;
        Array< Light * > m_directionalLights;
        Array< Light * > m_pointLights;
        Array< Light * > m_spotlights;
    };

}

#endif
