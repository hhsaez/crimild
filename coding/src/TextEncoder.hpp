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

#ifndef CRIMILD_CORE_CODING_TEXT_ENCODER_
#define CRIMILD_CORE_CODING_TEXT_ENCODER_

#include "Codable.hpp"
#include "EncodedData.hpp"
#include "Encoder.hpp"

#include <Crimild_Foundation.hpp>
#include <Crimild_Mathematics.hpp>

namespace crimild {

   namespace coding {

      class TextEncoder : public Encoder {
      public:
         virtual ~TextEncoder( void ) noexcept = default;

      public:
         virtual crimild::Bool encode( SharedPointer< Codable > const &obj ) override;
         virtual crimild::Bool encode( std::string key, SharedPointer< Codable > const &obj ) override;

         virtual crimild::Bool encode( std::string key, std::string value ) override;

         virtual crimild::Bool encode( std::string key, const Transformation &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, crimild::Size value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, crimild::UInt8 value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, crimild::UInt16 value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, crimild::Int16 value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, crimild::Int32 value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, crimild::UInt32 value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, crimild::Real32 value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, crimild::Real64 value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, const ColorRGB &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, const ColorRGBA &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, const Point2f &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, const Point3f &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, const Vector2f &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, const Vector3f &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, const Vector4f &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, const Matrix3f &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, const Matrix4f &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, const Quaternion &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, crimild::Bool value ) override { return encodeData( key, value ); }
         // virtual crimild::Bool encode( std::string key, const Format &value ) override { return encodeData( key, value ); }
         // virtual crimild::Bool encode( std::string key, const Extent2D &value ) override { return encodeData( key, value ); }
         // virtual crimild::Bool encode( std::string key, const Extent3D &value ) override { return encodeData( key, value ); }

         virtual bool encode( std::string_view key, std::vector< std::byte > &value ) override
         {
            // TODO: Use std::vector< std::byte > directly instead of converting it to encoded data
            Array< Byte > data;
            data.resize( value.size() );
            memcpy( data.getData(), value.data(), value.size() );
            auto encoded = crimild::alloc< EncodedData >( data );
            return encode( std::string( key ), encoded );
         }

         virtual crimild::Bool encode( std::string key, ByteArray &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, Array< crimild::Real32 > &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, Array< Vector3f > &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, Array< Vector4f > &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, Array< Matrix3f > &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, Array< Matrix4f > &value ) override { return encodeData( key, value ); }
         virtual crimild::Bool encode( std::string key, Array< Quaternion > &value ) override { return encodeData( key, value ); }

         std::string getString( void ) const;

      protected:
         virtual void encodeArrayBegin( std::string key, crimild::Size count ) override;
         virtual std::string beginEncodingArrayElement( std::string key, crimild::Size index ) override;
         virtual void endEncodingArrayElement( std::string key, crimild::Size index ) override;
         virtual void encodeArrayEnd( std::string key ) override;

      private:
         template< typename T >
         crimild::Bool encodeData( std::string key, const T &value )
         {
            auto encoded = crimild::alloc< EncodedData >( value );
            return encode( key, crimild::cast_ptr< EncodedData >( encoded ) );
         }

         static void append( ByteArray &out, crimild::Int8 value );
         static void append( ByteArray &out, std::string value );
         static void append( ByteArray &out, Codable::UniqueID value );
         static void append( ByteArray &out, const ByteArray &data );
         static void appendRawBytes( ByteArray &out, crimild::Size count, const void *data );

      private:
         Stack< SharedPointer< Codable > > _sortedObjects;
         Map< Codable::UniqueID, Map< std::string, Codable::UniqueID > > _links;
         SharedPointer< Codable > _parent;
         Array< SharedPointer< Codable > > _roots;

      public:
         virtual std::string dump( void ) override;
      };

   }

}

#endif
