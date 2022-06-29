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

#ifndef CRIMILD_CORE_CODING_ENCODER_
#define CRIMILD_CORE_CODING_ENCODER_

#include "Foundation/Containers/Array.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Foundation/Memory.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/Types.hpp"
#include "Foundation/Version.hpp"
#include "Mathematics/ColorRGB.hpp"
#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/Matrix3.hpp"
#include "Mathematics/Point2.hpp"
#include "Mathematics/Point3.hpp"
#include "Mathematics/Quaternion.hpp"
#include "Mathematics/Transformation.hpp"
#include "Mathematics/Vector2.hpp"
#include "Rendering/Extent.hpp"
#include "Rendering/Format.hpp"
#include "Rendering/VertexAttribute.hpp"

#include <sstream>

namespace crimild {

    namespace coding {

        class Codable;

        /**
         * \brief Base class for encoders
         *
         * \todo Encoders should get const versions of objects to be encoded.
         * \todo Should not retain pointers either.
         */
        class Encoder : public SharedObject {
        protected:
            Encoder( void ) = default;

        public:
            virtual ~Encoder( void ) = default;

            const Version &getVersion( void ) const { return _version; }
            void setVersion( const Version &version ) { _version = version; }

        private:
            Version _version;

        public:
            // objects
            virtual crimild::Bool encode( SharedPointer< Codable > const &codable ) = 0;
            crimild::Bool encode( std::string key, Codable *codable );
            virtual crimild::Bool encode( std::string key, SharedPointer< Codable > const &codable ) = 0;

            // values
            virtual crimild::Bool encode( std::string key, std::string str ) = 0;
            virtual crimild::Bool encode( std::string key, crimild::Size value ) = 0;
            virtual crimild::Bool encode( std::string key, crimild::UInt8 value ) = 0;
            virtual crimild::Bool encode( std::string key, crimild::UInt16 value ) = 0;
            virtual crimild::Bool encode( std::string key, crimild::Int16 value ) = 0;
            virtual crimild::Bool encode( std::string key, crimild::Int32 value ) = 0;
            virtual crimild::Bool encode( std::string key, crimild::UInt32 value ) = 0;
            virtual crimild::Bool encode( std::string key, crimild::Bool value ) = 0;
            virtual crimild::Bool encode( std::string key, crimild::Real32 value ) = 0;
            virtual crimild::Bool encode( std::string key, crimild::Real64 value ) = 0;
            virtual crimild::Bool encode( std::string key, const ColorRGB & ) = 0;
            virtual crimild::Bool encode( std::string key, const ColorRGBA & ) = 0;
            virtual crimild::Bool encode( std::string key, const Point2f & ) = 0;
            virtual crimild::Bool encode( std::string key, const Point3f & ) = 0;
            virtual crimild::Bool encode( std::string key, const Vector2f & ) = 0;
            virtual crimild::Bool encode( std::string key, const Vector3f & ) = 0;
            virtual crimild::Bool encode( std::string key, const Vector4f & ) = 0;
            virtual crimild::Bool encode( std::string key, const Matrix3f & ) = 0;
            virtual crimild::Bool encode( std::string key, const Matrix4f & ) = 0;
            virtual crimild::Bool encode( std::string key, const Quaternion & ) = 0;
            virtual crimild::Bool encode( std::string key, const Transformation & ) = 0;
            virtual crimild::Bool encode( std::string key, const Format & ) = 0;
            virtual crimild::Bool encode( std::string key, const Extent3D & ) = 0;

            virtual crimild::Bool encode( std::string key, const VertexAttribute &attr )
            {
                return encode( key + "_name", Int32( attr.name ) )
                    && encode( key + "_format", attr.format )
                    && encode( key + "_offset", attr.offset );
            }

            virtual bool encode( std::string_view key, std::vector< std::byte > & ) = 0;

            virtual crimild::Bool encode( std::string key, ByteArray & ) = 0;
            virtual crimild::Bool encode( std::string key, Array< crimild::Real32 > & ) = 0;
            virtual crimild::Bool encode( std::string key, Array< Vector3f > & ) = 0;
            virtual crimild::Bool encode( std::string key, Array< Vector4f > & ) = 0;
            virtual crimild::Bool encode( std::string key, Array< Matrix3f > & ) = 0;
            virtual crimild::Bool encode( std::string key, Array< Matrix4f > & ) = 0;
            virtual crimild::Bool encode( std::string key, Array< Quaternion > & ) = 0;

            template< typename T, typename U >
            crimild::Bool encode( std::string key, Array< T, U > &a )
            {
                crimild::Size N = a.size();
                encodeArrayBegin( key, N );

                a.each( [ this, key, i = 0 ]( T &elem ) mutable {
                    auto itemKey = beginEncodingArrayElement( key, i );
                    encode( itemKey, elem );
                    endEncodingArrayElement( key, i );
                    i++;
                } );

                encodeArrayEnd( key );

                return true;
            }

            template< typename EnumType >
            crimild::Bool encodeEnum( std::string key, const EnumType &value )
            {
                return encode( key, Int32( value ) );
            }

        protected:
            virtual void encodeArrayBegin( std::string key, crimild::Size count ) = 0;
            virtual std::string beginEncodingArrayElement( std::string key, crimild::Size index ) = 0;
            virtual void endEncodingArrayElement( std::string key, crimild::Size index ) = 0;
            virtual void encodeArrayEnd( std::string key ) = 0;

        public:
            virtual std::string dump( void );
        };
    }

}

#endif
