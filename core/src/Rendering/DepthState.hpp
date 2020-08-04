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

#ifndef CRIMILD_RENDERING_DEPTH_STATE_
#define CRIMILD_RENDERING_DEPTH_STATE_

#include "RenderState.hpp"

namespace crimild {

	class [[deprecated]] DepthState : public RenderState {
    public:
        // TODO: these should be 'const'
        static SharedPointer< DepthState > DISABLED;
        static SharedPointer< DepthState > ENABLED;

    public:
        enum class CompareFunc : uint8_t {
            NEVER,
            LESS,
            EQUAL,
            LEQUAL,
            GREATER,
            NOTEQUAL,
            GEQUAL,
            ALWAYS
        };

	public:
        explicit DepthState( bool enabled = true, CompareFunc compareFunc = CompareFunc::LESS, bool writable = true );
		virtual ~DepthState( void );

    public:
        void setCompareFunc( CompareFunc const &compareFunc ) { _compareFunc = compareFunc; }
        CompareFunc getCompareFunc( void ) const { return _compareFunc; }

    private:
        CompareFunc _compareFunc = CompareFunc::LESS;

    public:
        void setWritable( bool writable ) { _writable = writable; }
        bool isWritable( void ) const { return _writable; }

    private:
        bool _writable = true;

	public:
		inline void setBiasEnabled( bool enabled ) noexcept { _biasEnabled = enabled; }
		inline bool isBiasEnabled( void ) const noexcept { return _biasEnabled; }

		inline void setBiasConstantFactor( float factor ) noexcept { _biasConstantFactor = factor; }
		inline float getBiasConstantFactor( void ) const noexcept { return _biasConstantFactor; }

		inline void setBiasClamp( float clamp ) noexcept { _biasClamp = clamp; }
		inline float getBiasClamp( void ) const noexcept { return _biasClamp; }

		inline void setBiasSlopeFactor( float factor ) noexcept { _biasSlopeFactor = factor; }
		inline float getBiasSlopeFactor( void ) const noexcept { return _biasSlopeFactor; }

	private:
		bool _biasEnabled = false;
		float _biasConstantFactor = 0.0f;
		float _biasClamp = 0.0f;
		float _biasSlopeFactor = 0.0f;
	};

}

#endif
