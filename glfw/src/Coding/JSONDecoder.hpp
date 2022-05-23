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

#ifndef CRIMILD_DESKTOP_CODING_JSON_DECODER_
#define CRIMILD_DESKTOP_CODING_JSON_DECODER_

#include "Coding/Decoder.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Foundation/JSONUtils.hpp"
#include "Foundation/Log.hpp"

namespace crimild {

    namespace coding {

        class JSONDecoder : public Decoder {
        public:
            Bool fromJSON( const nlohmann::json &json );
            Bool fromFile( std::string path );

            virtual crimild::Bool decode( std::string key, SharedPointer< coding::Codable > &codable ) override;

            virtual crimild::Bool decode( std::string key, std::string &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Size &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::UInt8 &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::UInt16 &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Int16 &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Int32 &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::UInt32 &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Bool &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Real32 &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Real64 &value ) override { return decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::ColorRGB &value ) override { return decodeData( key, 3, static_cast< float * >( &value.r ) ); }
            virtual crimild::Bool decode( std::string key, crimild::ColorRGBA &value ) override { return decodeData( key, 4, static_cast< float * >( &value.r ) ); }
            virtual crimild::Bool decode( std::string key, crimild::Point2f &value ) override { return decodeData( key, 2, static_cast< float * >( &value.x ) ); }
            virtual crimild::Bool decode( std::string key, crimild::Point3f &value ) override { return decodeData( key, 3, static_cast< float * >( &value.x ) ); }
            virtual crimild::Bool decode( std::string key, crimild::Vector2f &value ) override { return decodeData( key, 2, static_cast< float * >( &value.x ) ); }
            virtual crimild::Bool decode( std::string key, crimild::Vector3f &value ) override { return decodeData( key, 3, static_cast< float * >( &value.x ) ); }
            virtual crimild::Bool decode( std::string key, crimild::Vector4f &value ) override { return decodeData( key, 4, static_cast< float * >( &value.x ) ); }
            virtual crimild::Bool decode( std::string key, crimild::Matrix3f &value ) override { return decodeData( key, 9, static_cast< float * >( &value.c0.x ) ); }
            virtual crimild::Bool decode( std::string key, crimild::Matrix4f &value ) override { return decodeData( key, 16, static_cast< float * >( &value.c0.x ) ); }
            virtual crimild::Bool decode( std::string key, crimild::Quaternion &value ) override { return false; } //decodeData( key, value ); }
            virtual crimild::Bool decode( std::string key, Transformation &value ) override;
            virtual crimild::Bool decode( std::string key, Format &value ) override { return decodeData( key, value ); }

            virtual bool decode( std::string_view key, std::vector< std::byte > &value ) override
            {
                // TODO
                return false;
            }

            virtual crimild::Bool decode( std::string key, ByteArray &value ) override { return decodeDataArray< Byte >( key, value, 1 ); }
            virtual crimild::Bool decode( std::string key, Array< crimild::Real32 > &value ) override { return decodeDataArray< Real >( key, value, 1 ); }
            virtual crimild::Bool decode( std::string key, Array< Vector3f > &value ) override { return decodeDataArray< Real >( key, value, 3 ); }
            virtual crimild::Bool decode( std::string key, Array< Vector4f > &value ) override { return decodeDataArray< Real >( key, value, 4 ); }
            virtual crimild::Bool decode( std::string key, Array< Matrix3f > &value ) override { return decodeDataArray< Real >( key, value, 9 ); }
            virtual crimild::Bool decode( std::string key, Array< Matrix4f > &value ) override { return decodeDataArray< Real >( key, value, 16 ); }
            virtual crimild::Bool decode( std::string key, Array< Quaternion > &value ) override { return false; } //decodeDataArray( key, value ); }

        private:
            template< typename T >
            crimild::Bool decodeData( const std::string &key, T &value )
            {
                const auto id = m_currentObj->getUniqueID();
                if ( !m_encoded.contains( id ) ) {
                    CRIMILD_LOG_ERROR( "Cannot decode value with key \"", key, "\" from unrecognized object with id ", id );
                    return false;
                }
                const auto &json = m_encoded[ id ];
                value = json.at( key ).get< T >();
                return true;
            }

            template< typename T >
            crimild::Bool decodeData( const std::string &key, Size count, T *values )
            {
                const auto id = m_currentObj->getUniqueID();
                if ( !m_encoded.contains( id ) ) {
                    CRIMILD_LOG_ERROR( "Cannot decode value with key \"", key, "\" from unrecognized object with id ", id );
                    return false;
                }
                const auto &json = m_encoded[ id ];
                if ( json[ key ].size() != count ) {
                    CRIMILD_LOG_ERROR( "Invalid array size for key \"", key, "\" (", json.size(), " != ", count, ")" );
                    return false;
                }

                auto i = Size( 0 );
                for ( const auto &it : json[ key ] ) {
                    values[ i++ ] = it.get< T >();
                }

                return true;
            }

            template< typename T, typename ArrayType >
            crimild::Bool decodeDataArray( std::string key, ArrayType &values, Size stride )
            {
                const auto id = m_currentObj->getUniqueID();
                if ( !m_encoded.contains( id ) ) {
                    CRIMILD_LOG_ERROR( "Cannot decode value with key \"", key, "\" from unrecognized object with id ", id );
                    return false;
                }
                const auto &json = m_encoded[ id ];

                const auto N = json[ key ].size() / stride;
                values.resize( N );
                auto dataPtr = static_cast< T * >( static_cast< void * >( values.getData() ) );
                auto i = Size( 0 );
                for ( const auto &it : json[ key ] ) {
                    dataPtr[ i++ ] = it.get< T >();
                }

                return true;
            }

        protected:
            virtual crimild::Size beginDecodingArray( std::string key ) override;
            virtual std::string beginDecodingArrayElement( std::string key, crimild::Size index ) override;
            virtual void endDecodingArrayElement( std::string key, crimild::Size index ) override;
            virtual void endDecodingArray( std::string key ) override;

            // private:
            //     static crimild::Size read( const ByteArray &bytes, crimild::Int8 &value, crimild::Size offset );
            //     static crimild::Size read( const ByteArray &bytes, Codable::UniqueID &value, crimild::Size offset );
            //     static crimild::Size read( const ByteArray &bytes, std::string &value, crimild::Size offset );
            //     static crimild::Size read( const ByteArray &bytes, ByteArray &value, crimild::Size offset );
            //     static crimild::Size readRawBytes( const ByteArray &bytes, void *data, crimild::Size count, crimild::Size offset );

        private:
            Map< Codable::UniqueID, Map< std::string, SharedPointer< Codable > > > m_links;
            Map< Codable::UniqueID, SharedPointer< Codable > > m_objects;
            Map< Codable::UniqueID, nlohmann::json > m_encoded;
            SharedPointer< Codable > m_currentObj;
        };

    }

}

#endif
