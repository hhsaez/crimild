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

#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Vector4.hpp"
#include "Rendering/UniformBuffer.hpp"

namespace crimild {

    class Light;

    class [[deprecated]] LightUniform : public UniformBuffer {
    public:
        struct LightProps {
            alignas( 4 ) UInt32 type;
            alignas( 16 ) Vector4f position;
            alignas( 16 ) Vector4f direction;
            alignas( 16 ) ColorRGBA ambient;
            alignas( 16 ) ColorRGBA color;
            alignas( 16 ) Vector4f attenuation;
            alignas( 16 ) Vector4f cutoff;
            alignas( 4 ) UInt32 castShadows;
            alignas( 4 ) Real32 shadowBias;
            alignas( 16 ) Vector4f cascadeSplits;
            alignas( 16 ) Matrix4f lightSpaceMatrix[ 4 ];
            alignas( 16 ) Vector4f viewport;
            alignas( 4 ) Real32 energy;
            alignas( 4 ) Real32 radius;
        };

    public:
        explicit LightUniform( Light *light ) noexcept;
        virtual ~LightUniform( void ) = default;

        void onPreRender( void ) noexcept override;

    private:
        Light *m_light = nullptr;
    };

}

#endif
