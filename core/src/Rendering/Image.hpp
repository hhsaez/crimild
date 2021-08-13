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

#ifndef CRIMILD_RENDERING_IMAGE_
#define CRIMILD_RENDERING_IMAGE_

#include "Coding/Codable.hpp"
#include "Foundation/Containers/Array.hpp"
#include "Foundation/NamedObject.hpp"
#include "Rendering/BufferView.hpp"
#include "Rendering/Extent.hpp"
#include "Rendering/Format.hpp"
#include "Rendering/FrameGraphResource.hpp"
#include "Rendering/RenderResource.hpp"

#include <vector>

namespace crimild {

    class Image
        : public coding::Codable,
          public NamedObject,
          public RenderResourceImpl< Image >,
          public FrameGraphResource {
        CRIMILD_IMPLEMENT_RTTI( crimild::Image )

    public:
        static SharedPointer< Image > ONE;
        static SharedPointer< Image > CUBE_ONE;
        static SharedPointer< Image > ZERO;
        static SharedPointer< Image > CHECKERBOARD;
        static SharedPointer< Image > CHECKERBOARD_4;
        static SharedPointer< Image > CHECKERBOARD_8;
        static SharedPointer< Image > CHECKERBOARD_16;
        static SharedPointer< Image > CHECKERBOARD_32;
        static SharedPointer< Image > CHECKERBOARD_64;
        static SharedPointer< Image > CHECKERBOARD_128;
        static SharedPointer< Image > CHECKERBOARD_256;
        static SharedPointer< Image > CHECKERBOARD_512;
        static SharedPointer< Image > INVALID;

        static SharedPointer< Image > fromRGBANoise( UInt32 size = 256 ) noexcept;

    public:
        enum class Type {
            IMAGE_1D,
            IMAGE_2D,
            IMAGE_2D_CUBEMAP,
            IMAGE_3D,
        };

    public:
        inline FrameGraphResource::Type getType( void ) const noexcept override { return FrameGraphResource::Type::IMAGE; }

        Format format = Format::UNDEFINED;
        Type type = Type::IMAGE_2D;

        /**
		   \brief image size

		   For 1D image, height and depth must be 1
		   For 2D image, depth must be 1
		 */
        Extent3D extent;

        /**
         * \name Buffer View
         */
        //@{
    public:
        inline void setBufferView( SharedPointer< BufferView > const &bufferView ) noexcept { m_bufferView = bufferView; }
        inline BufferView *getBufferView( void ) noexcept { return crimild::get_ptr( m_bufferView ); }

    private:
        SharedPointer< BufferView > m_bufferView;

        //@}

        /**
           \name Layer count
         */
        //@{

    public:
        inline void setLayerCount( UInt32 layerCount ) noexcept { m_layerCount = layerCount; }
        inline UInt32 getLayerCount( void ) const noexcept { return m_layerCount; }

    private:
        UInt32 m_layerCount = 1; //< at least 1 layer

        //@}

        /**
         	\name Mipmapping

         	Auto generated mipmapping is enabled by default (mipLevels = 0)
         */
        //@{

    public:
        void setMipLevels( crimild::UInt32 mipLevels ) noexcept;
        crimild::UInt32 getMipLevels( void ) const noexcept;

    private:
        crimild::UInt32 m_mipLevels = 0;

        //@}

        /**
        \name Coding support
     */
        //@{

    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;

        //@}

        /**
		\name Deprecated
    */
        //@{

    public:
        enum class PixelFormat {
            RGB,
            RGBA,
            BGR,
            BGRA,
            RED,
            DEPTH_16,
            DEPTH_24,
            DEPTH_32,
        };

        enum class PixelType {
            UNSIGNED_BYTE,
            FLOAT,
        };

    public:
        Image( void );
        Image( int width, int height, int bpp, PixelFormat format, PixelType pixelType );
        Image( int width, int height, int bpp, const unsigned char *data, PixelFormat format = PixelFormat::RGBA );
        Image( int width, int height, int bpp, const ByteArray &data, PixelFormat format = PixelFormat::RGBA );
        virtual ~Image( void );

        int getWidth( void ) const { return _width; }
        int getHeight( void ) const { return _height; }
        int getBpp( void ) const { return _bpp; }
        PixelFormat getPixelFormat( void ) const { return _pixelFormat; }
        PixelType getPixelType( void ) const { return _pixelType; }

        crimild::Bool hasData( void ) const { return _data.size(); }
        unsigned char *getData( void ) { return _data.getData(); }
        const unsigned char *getData( void ) const { return _data.getData(); }
        void setData( int width, int height, int bpp, const unsigned char *data, PixelFormat format = PixelFormat::RGBA, PixelType pixelType = PixelType::UNSIGNED_BYTE );

        bool isLoaded( void ) const { return _data.size() > 0; }
        virtual void load( void );
        virtual void unload( void );

    private:
        int _width;
        int _height;
        int _bpp;
        PixelFormat _pixelFormat;
        PixelType _pixelType;
        ByteArray _data;

        //@}
    };

}

#endif
