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

#ifndef CRIMILD_CORE_CODING_MEMORY_DECODER_
#define CRIMILD_CORE_CODING_MEMORY_DECODER_

#include "Decoder.hpp"
#include "EncodedData.hpp"
#include "Foundation/Containers/Map.hpp"

namespace crimild {

    namespace coding {

        class MemoryDecoder : public Decoder {
        public:
            MemoryDecoder( void );
            virtual ~MemoryDecoder( void );

        public:
            virtual crimild::Bool decode( std::string key, SharedPointer< coding::Codable > &codable ) override;

            virtual crimild::Bool decode( std::string key, std::string &value ) override;

            virtual crimild::Bool decode( std::string key, crimild::Size &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::UInt8 &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::UInt16 &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Int16 &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Int32 &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::UInt32 &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Bool &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Real32 &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Real64 &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::ColorRGB &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::ColorRGBA &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Point2f &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Point3f &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Vector2f &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Vector3f &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Vector4f &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Matrix3f &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Matrix4f &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Quaternion &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, Transformation &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, Format &value ) override { return decodeData( key, value ); }

            virtual crimild::Bool decode( std::string key, ByteArray &value ) override { return decodeDataArray( key, value ); }
            virtual crimild::Bool decode( std::string key, Array< crimild::Real32 > &value ) override { return decodeDataArray( key, value ); }
            virtual crimild::Bool decode( std::string key, Array< Vector3f > &value ) override { return decodeDataArray( key, value ); }
            virtual crimild::Bool decode( std::string key, Array< Vector4f > &value ) override { return decodeDataArray( key, value ); }
            virtual crimild::Bool decode( std::string key, Array< Matrix3f > &value ) override { return decodeDataArray( key, value ); }
            virtual crimild::Bool decode( std::string key, Array< Matrix4f > &value ) override { return decodeDataArray( key, value ); }
            virtual crimild::Bool decode( std::string key, Array< Quaternion > &value ) override { return decodeDataArray( key, value ); }

            crimild::Bool fromBytes( const ByteArray &bytes );

        private:
            template< typename T >
            crimild::Bool decodeData( std::string key, T &value )
            {
                auto obj = crimild::cast_ptr< EncodedData >( _links[ _currentObj->getUniqueID() ][ key ] );
                if ( obj == nullptr ) {
                    value = T();
                    return false;
                }

                value = obj->getValue< T >();

                return true;
            }

            template< typename T >
            crimild::Bool decodeDataArray( std::string key, Array< T > &value )
            {
                auto obj = crimild::cast_ptr< EncodedData >( _links[ _currentObj->getUniqueID() ][ key ] );
                if ( obj == nullptr ) {
                    return false;
                }

                const auto N = obj->getBytes().size() / sizeof( T );
                value.resize( N );
                if ( N > 0 ) {
                    memcpy( &value[ 0 ], obj->getBytes().getData(), obj->getBytes().size() );
                }

                return true;
            }

        protected:
            virtual crimild::Size beginDecodingArray( std::string key ) override;
            virtual std::string beginDecodingArrayElement( std::string key, crimild::Size index ) override;
            virtual void endDecodingArrayElement( std::string key, crimild::Size index ) override;
            virtual void endDecodingArray( std::string key ) override;

        private:
            static crimild::Size read( const ByteArray &bytes, crimild::Int8 &value, crimild::Size offset );
            static crimild::Size read( const ByteArray &bytes, Codable::UniqueID &value, crimild::Size offset );
            static crimild::Size read( const ByteArray &bytes, std::string &value, crimild::Size offset );
            static crimild::Size read( const ByteArray &bytes, ByteArray &value, crimild::Size offset );
            static crimild::Size readRawBytes( const ByteArray &bytes, void *data, crimild::Size count, crimild::Size offset );

        private:
            Map< Codable::UniqueID, Map< std::string, SharedPointer< Codable > > > _links;
            Map< Codable::UniqueID, SharedPointer< Codable > > _objects;
            SharedPointer< Codable > _currentObj;
        };

    }

}

#endif
