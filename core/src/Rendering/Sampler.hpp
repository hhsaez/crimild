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

#ifndef CRIMILD_RENDERING_SAMPLER_
#define CRIMILD_RENDERING_SAMPLER_

#include "Coding/Codable.hpp"
#include "Rendering/CompareOp.hpp"
#include "Rendering/RenderResource.hpp"

namespace crimild {

    class Sampler :
		public coding::Codable,
    	public RenderResourceImpl< Sampler > {
        CRIMILD_IMPLEMENT_RTTI( crimild::Sampler )

	public:
        enum class WrapMode : uint8_t {
            REPEAT,         //< Default
			MIRRORED_REPEAT,
            CLAMP_TO_EDGE,
			CLAMP_TO_BORDER,
        };

        WrapMode getWrapMode( void ) const { return m_wrapMode; }
        void setWrapMode( const WrapMode &mode ) { m_wrapMode = mode; }

    private:
        WrapMode m_wrapMode = WrapMode::REPEAT;

    public:
        enum class Filter : uint8_t {
            NEAREST,
            LINEAR,                     //< Default
            NEAREST_MIPMAP_NEAREST,
            NEAREST_MIPMAP_LINEAR,
            LINEAR_MIPMAP_NEAREST,
            LINEAR_MIPMAP_LINEAR
        };

        void setMinFilter( Filter filter ) { m_minFilter = filter; }
        Filter getMinFilter( void ) const { return m_minFilter; }

        void setMagFilter( Filter filter ) { m_magFilter = filter; }
        Filter getMagFilter( void ) const { return m_magFilter; }

    private:
        Filter m_minFilter = Filter::LINEAR;
        Filter m_magFilter = Filter::LINEAR;

        /**
            \name Border color
         */
        //@{

    public:
        enum class BorderColor {
			FLOAT_TRANSPARENT_BLACK,
            INT_TRANSPARENT_BLACK,
            FLOAT_OPAQUE_BLACK,
            INT_OPAQUE_BLACK,
            FLOAT_OPAQUE_WHITE,
            INT_OPAQUE_WHITE,
        };
        void setBorderColor( BorderColor color ) { m_borderColor = color; }
        BorderColor getBorderColor( void ) const { return m_borderColor; }

    private:
        BorderColor m_borderColor = BorderColor::INT_OPAQUE_BLACK;

        /**
		   \name Level of detail

		   Usually, set minLod to 0 and maxLod to image->getMipLevels()
         */
		//@{

	public:
		void setMinLod( crimild::Real32 minLod ) noexcept { m_minLod = minLod; }
		crimild::Real32 getMinLod( void ) const noexcept { return m_minLod; }

		void setMaxLod( crimild::Real32 maxLod ) noexcept { m_maxLod = maxLod; }
		crimild::Real32 getMaxLod( void ) const noexcept { return m_maxLod; }

	private:
		crimild::Real32 m_minLod = 0.0f;
		crimild::Real32 m_maxLod = 1.0f;

		//@}

        /**
           \name Compare
         */
        //@{

    public:
        inline void setCompareOp( CompareOp compare ) noexcept { m_compareOp = compare; }
        inline CompareOp getCompareOp( void ) const noexcept { return m_compareOp; }

    private:
        CompareOp m_compareOp = CompareOp::ALWAYS;

        //@}

        /**
            \name Coding support
         */
        //@{

    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;

        //@}

    };

}

#endif
