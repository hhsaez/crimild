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

#ifndef CRIMILD_RENDERING_TEXTURE_
#define CRIMILD_RENDERING_TEXTURE_

#include "Image.hpp"
#include "Catalog.hpp"

#include "Foundation/NamedObject.hpp"

namespace crimild {
    
    class Texture : public NamedObject, public coding::Codable, public StreamObject, public Catalog< Texture >::Resource {
        CRIMILD_IMPLEMENT_RTTI( crimild::Texture )

	public:
		static SharedPointer< Texture > ONE;
		static SharedPointer< Texture > ZERO;

	public:
        explicit Texture( std::string name = "ColorMap" );
		explicit Texture( SharedPointer< Image > const &image, std::string name = "ColorMap" );
		virtual ~Texture( void );

    public:
        Image *getImage( void ) { return crimild::get_ptr( _image ); }
        
	private:
		SharedPointer< Image > _image;

    public:
        enum class WrapMode : uint8_t {
            REPEAT,         //< Default
            CLAMP_TO_EDGE   
        };
        
        WrapMode getWrapMode( void ) const { return _wrapMode; }
        void setWrapMode( const WrapMode &mode ) { _wrapMode = mode; }
        
    private:
        WrapMode _wrapMode = WrapMode::REPEAT;

    public:
        enum class Filter : uint8_t {
            NEAREST,
            LINEAR,                     //< Default
            NEAREST_MIPMAP_NEAREST,
            NEAREST_MIPMAP_LINEAR,
            LINEAR_MIPMAP_NEAREST,
            LINEAR_MIPMAP_LINEAR
        };

        void setMinFilter( Filter filter ) { _minFilter = filter; }
        Filter getMinFilter( void ) const { return _minFilter; }

        void setMagFilter( Filter filter ) { _magFilter = filter; }
        Filter getMagFilter( void ) const { return _magFilter; }

    private:
        Filter _minFilter = Filter::LINEAR;
        Filter _magFilter = Filter::LINEAR;

        /**
            \name Coding support
         */
        //@{
        
    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;
        
        //@}
        
        /**
            \name Streaming
        */
        //@{

    public:
        virtual bool registerInStream( Stream &s ) override;
        virtual void save( Stream &s ) override;
        virtual void load( Stream &s ) override;

        //@}
    };
	
}

#endif

