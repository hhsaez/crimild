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

#ifndef CRIMILD_RENDERING_TEXTURE_
#define CRIMILD_RENDERING_TEXTURE_

#include "Catalog.hpp"
#include "Foundation/NamedObject.hpp"
#include "Image.hpp"
#include "Rendering/FrameGraphResource.hpp"
#include "Rendering/RenderResource.hpp"

namespace crimild {

    class ImageView;
    class Sampler;

    class Texture
        : public NamedObject,
          public coding::Codable,
          public RenderResourceImpl< Texture >,
          public Catalog< Texture >::Resource,
          public FrameGraphResource {
        CRIMILD_IMPLEMENT_RTTI( crimild::Texture )

    public:
        static SharedPointer< Texture > fromRGBANoise( UInt32 size = 256 ) noexcept;

    public:
        /**
		   \brief Destructor
		 */
        virtual ~Texture( void );

        inline FrameGraphResource::Type getType( void ) const noexcept override { return FrameGraphResource::Type::TEXTURE; }

        SharedPointer< ImageView > imageView;
        SharedPointer< Sampler > sampler;

        virtual void setWrittenBy( FrameGraphOperation *op ) noexcept override;
        virtual void setReadBy( FrameGraphOperation *op ) noexcept override;

        // DEPRECATED FROM HERE?

    public:
        static SharedPointer< Texture > ONE;
        static SharedPointer< Texture > CUBE_ONE;
        static SharedPointer< Texture > ZERO;
        static SharedPointer< Texture > CHECKERBOARD;
        static SharedPointer< Texture > CHECKERBOARD_4;
        static SharedPointer< Texture > CHECKERBOARD_8;
        static SharedPointer< Texture > CHECKERBOARD_16;
        static SharedPointer< Texture > CHECKERBOARD_32;
        static SharedPointer< Texture > CHECKERBOARD_64;
        static SharedPointer< Texture > CHECKERBOARD_128;
        static SharedPointer< Texture > CHECKERBOARD_256;
        static SharedPointer< Texture > CHECKERBOARD_512;
        static SharedPointer< Texture > INVALID;

        struct Target {
            enum {
                TEXTURE_2D,
                CUBE_MAP,
            };

            using Impl = crimild::UInt8;
        };

        struct CubeMapFace {
            enum {
                RIGHT,
                LEFT,
                TOP,
                BOTTOM,
                BACK,
                FRONT
            };

            using Impl = crimild::UInt8;
        };

    private:
        using ImageArray = Array< SharedPointer< Image > >;

    public:
        /**
		   \brief Construct an empty TEXTURE_2D texture
		 */
        explicit Texture( std::string name = "ColorMap" );

        /**
		   \brief Construct a TEXTURE_2D texture with an image
		 */
        explicit Texture( SharedPointer< Image > const &image, std::string name = "ColorMap" );

        /**
		   \brief Construct a CUBE_MAP texture

		   \remarks Faces: Right, Left, Top, Bottom, Back, Front
		 */
        explicit Texture( ImageArray const &faces );

        inline Target::Impl getTarget( void ) const { return _target; }

    private:
        Target::Impl _target = Target::TEXTURE_2D;

    public:
        inline Image *getImage( void )
        {
            if ( _images.empty() ) {
                return nullptr;
            }

            return crimild::get_ptr( _images[ 0 ] );
        }

        inline Image *getFace( CubeMapFace::Impl faceId )
        {
            if ( faceId >= _images.size() ) {
                return nullptr;
            }

            return crimild::get_ptr( _images[ faceId ] );
        }

    private:
        ImageArray _images;

    public:
        enum class WrapMode : uint8_t {
            REPEAT, //< Default
            MIRRORED_REPEAT,
            CLAMP_TO_EDGE,
            CLAMP_TO_BORDER,
        };

        WrapMode getWrapMode( void ) const { return _wrapMode; }
        void setWrapMode( const WrapMode &mode ) { _wrapMode = mode; }

    private:
        WrapMode _wrapMode = WrapMode::REPEAT;

    public:
        enum class Filter : uint8_t {
            NEAREST,
            LINEAR, //< Default
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
        void setBorderColor( BorderColor color ) { _borderColor = color; }
        BorderColor getBorderColor( void ) const { return _borderColor; }

    private:
        BorderColor _borderColor = BorderColor::INT_OPAQUE_BLACK;

        /**
            \name Mipmapping
         */
        //@{
    public:
        void setMipmappingEnabled( crimild::Bool enabled ) noexcept { m_mipmappingEnabled = enabled; }
        crimild::Bool isMipmappingEnabled( void ) const noexcept { return m_mipmappingEnabled; }

    private:
        crimild::Bool m_mipmappingEnabled = true;

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
