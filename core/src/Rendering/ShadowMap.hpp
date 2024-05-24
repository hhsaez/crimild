/*
 * Copyright (c) 2013, Hernan Saez
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

#ifndef CRIMILD_CORE_RENDERING_SHADOW_MAP_
#define CRIMILD_CORE_RENDERING_SHADOW_MAP_

#include "Crimild_Foundation.hpp"
#include "Crimild_Mathematics.hpp"

namespace crimild {

    class CullFaceState;

    class [[deprecated]] ShadowMap : public SharedObject {
    public:
        ShadowMap( void );
        virtual ~ShadowMap( void );

        inline Vector4f getCascadeSplits( void ) const
        {
            return Vector4f {
                _cascadeSplits[ 0 ],
                _cascadeSplits[ 1 ],
                _cascadeSplits[ 2 ],
                _cascadeSplits[ 3 ]
            };
        }
        void setCascadeSplits( const Vector4f &splits ) noexcept
        {
            for ( auto i = 0; i < 4; ++i ) {
                _cascadeSplits[ i ] = splits[ i ];
            }
        }
        void setCascadeSplit( Size splitIndex, Real32 depth ) noexcept { _cascadeSplits[ splitIndex ] = depth; }

        const Matrix4f &getLightProjectionMatrix( Size index ) const { return _lightProjectionMatrix[ index ]; }
        void setLightProjectionMatrix( Size index, const Matrix4f &m ) { _lightProjectionMatrix[ index ] = m; }

        const Matrix4f &getLightViewMatrix( void ) const { return _lightViewMatrix; }
        void setLightViewMatrix( const Matrix4f &m ) { _lightViewMatrix = m; }

        const Vector4f &getViewport( void ) const { return _viewport; }
        void setViewport( const Vector4f &viewport ) { _viewport = viewport; }

        inline void setBias( Real32 bias ) noexcept { m_bias = bias; }
        inline Real32 getBias( void ) const noexcept { return m_bias; }

    private:
        Array< Real > _cascadeSplits = { 0, 0, 0, 0 };
        Matrix4f _lightProjectionMatrix[ 4 ];
        Matrix4f _lightViewMatrix;
        Vector4f _viewport;
        crimild::Real32 m_bias = 0.1f;
    };

}

#endif
