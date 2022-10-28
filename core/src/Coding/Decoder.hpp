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

#ifndef CRIMILD_CORE_CODING_DECODER_
#define CRIMILD_CORE_CODING_DECODER_

#include "Codable.hpp"
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
#include "Mathematics/Vector3.hpp"
#include "Rendering/Extent.hpp"
#include "Rendering/Format.hpp"
#include "Rendering/VertexAttribute.hpp"

namespace crimild {

    namespace coding {

        class Decoder : public SharedObject {
        protected:
            Decoder( void ) = default;

        public:
            virtual ~Decoder( void ) = default;

            const Version &getVersion( void ) const { return _version; }
            void setVersion( const Version &version ) { _version = version; }

        private:
            Version _version;

        public:
            virtual crimild::Bool decode( std::string key, SharedPointer< coding::Codable > &codable ) = 0;

            Bool decode( std::string key, Version &version ) noexcept
            {
                uint32_t major, minor, patch;
                decode( key + "_major", major );
                decode( key + "_minor", minor );
                decode( key + "_patch", patch );
                version = Version( major, minor, patch );
                return true;
            }

            template< class T >
            crimild::Bool decode( std::string key, SharedPointer< T > &obj )
            {
                auto codable = crimild::cast_ptr< coding::Codable >( obj );
                decode( key, codable );
                if ( codable == nullptr ) {
                    return false;
                }

                obj = crimild::cast_ptr< T >( codable );
                return true;
            }

            virtual crimild::Bool decode( std::string key, std::string &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::Size &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::UInt8 &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::UInt16 &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::Int16 &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::Int32 &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::UInt32 &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::Bool &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::Real32 &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::Real64 &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::ColorRGB &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::ColorRGBA &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::Point2f &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::Point3f &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::Vector2f &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::Vector3f &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::Vector4f &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::Matrix3f &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::Matrix4f &value ) = 0;
            virtual crimild::Bool decode( std::string key, crimild::Quaternion &value ) = 0;
            virtual crimild::Bool decode( std::string key, Transformation &value ) = 0;
            virtual crimild::Bool decode( std::string key, Format &value ) = 0;
            virtual crimild::Bool decode( std::string key, Extent3D &value ) = 0;

            virtual crimild::Bool decode( std::string key, VertexAttribute &attr )
            {
                Int32 name;
                decode( key + "_name", name );
                attr.name = VertexAttribute::Name( name );

                decode( key + "_format", attr.format );
                decode( key + "_offset", attr.offset );
                return true;
            }

            virtual bool decode( std::string_view key, std::vector< std::byte > & ) = 0;

            virtual crimild::Bool decode( std::string key, ByteArray &value ) = 0;
            virtual crimild::Bool decode( std::string key, Array< crimild::Real32 > &value ) = 0;
            virtual crimild::Bool decode( std::string key, Array< Vector3f > &value ) = 0;
            virtual crimild::Bool decode( std::string key, Array< Vector4f > &value ) = 0;
            virtual crimild::Bool decode( std::string key, Array< Matrix3f > &value ) = 0;
            virtual crimild::Bool decode( std::string key, Array< Matrix4f > &value ) = 0;
            virtual crimild::Bool decode( std::string key, Array< Quaternion > &value ) = 0;

            template< typename T >
            crimild::Bool decode( std::string key, Array< SharedPointer< T > > &value )
            {
                auto count = beginDecodingArray( key );

                value.clear();
                for ( crimild::Size i = 0; i < count; i++ ) {
                    auto v = SharedPointer< Codable >();
                    auto itemKey = beginDecodingArrayElement( key, i );
                    if ( decode( itemKey, v ) ) {
                        value.add( crimild::cast_ptr< T >( v ) );
                    }
                    endDecodingArrayElement( key, i );
                }

                endDecodingArray( key );

                return true;
            }

            template< typename T >
            crimild::Bool decode( std::string key, Array< T > &value )
            {
                auto count = beginDecodingArray( key );

                value.resize( count );
                for ( crimild::Size i = 0; i < count; i++ ) {
                    auto v = T();
                    auto itemKey = beginDecodingArrayElement( key, i );
                    decode( itemKey, v );
                    endDecodingArrayElement( key, i );
                    value[ i ] = v;
                }

                endDecodingArray( key );

                return true;
            }

            template< typename EnumType >
            crimild::Bool decodeEnum( std::string key, EnumType &value )
            {
                Int32 encoded;
                decode( key, encoded );
                value = EnumType( encoded );
                return true;
            }

            inline crimild::Size getObjectCount( void ) const
            {
                return _roots.size();
            }

            template< class T >
            inline SharedPointer< T > getObjectAt( crimild::Size index )
            {
                return crimild::cast_ptr< T >( _roots[ index ] );
            }

        protected:
            virtual crimild::Size beginDecodingArray( std::string key ) = 0;
            virtual std::string beginDecodingArrayElement( std::string key, crimild::Size index ) = 0;
            virtual void endDecodingArrayElement( std::string key, crimild::Size index ) = 0;
            virtual void endDecodingArray( std::string key ) = 0;

        protected:
            inline void addRootObject( SharedPointer< SharedObject > const &obj ) noexcept
            {
	            _roots.add( obj );
            }

        private:
            Array< SharedPointer< SharedObject > > _roots;
        };

    }

}

#endif
