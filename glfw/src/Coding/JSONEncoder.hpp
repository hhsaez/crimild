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

#ifndef CRIMILD_DESKTOP_CODING_JSON_ENCODER_
#define CRIMILD_DESKTOP_CODING_JSON_ENCODER_

#include "Coding/Codable.hpp"
#include "Coding/Encoder.hpp"
#include "Foundation/Containers/Stack.hpp"
#include "Foundation/JSONUtils.hpp"

namespace crimild {

    namespace coding {

        class JSONEncoder : public coding::Encoder {
        public:
            virtual crimild::Bool encode( SharedPointer< coding::Codable > const &codable ) override;
            virtual crimild::Bool encode( std::string key, SharedPointer< coding::Codable > const &codable ) override;

            virtual crimild::Bool encode( std::string key, std::string value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Size value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::UInt8 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::UInt16 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Int16 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Int32 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::UInt32 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Bool value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Real32 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Real64 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, const ColorRGB &value ) override { return encodeValues( key, 3, static_cast< const float * >( &value.r ) ); }
            virtual crimild::Bool encode( std::string key, const ColorRGBA &value ) override { return encodeValues( key, 4, static_cast< const float * >( &value.r ) ); }
            virtual crimild::Bool encode( std::string key, const Point2f &value ) override { return encodeValues( key, 2, static_cast< const float * >( &value.x ) ); }
            virtual crimild::Bool encode( std::string key, const Point3f &value ) override { return encodeValues( key, 3, static_cast< const float * >( &value.x ) ); }
            virtual crimild::Bool encode( std::string key, const Vector2f &value ) override { return encodeValues( key, 2, static_cast< const float * >( &value.x ) ); }
            virtual crimild::Bool encode( std::string key, const Vector3f &value ) override { return encodeValues( key, 3, static_cast< const float * >( &value.x ) ); }
            virtual crimild::Bool encode( std::string key, const Vector4f &value ) override { return encodeValues( key, 4, static_cast< const float * >( &value.x ) ); }
            virtual crimild::Bool encode( std::string key, const Matrix3f &value ) override { return encodeValues( key, 9, static_cast< const float * >( &value.c0.x ) ); }
            virtual crimild::Bool encode( std::string key, const Matrix4f &value ) override { return encodeValues( key, 16, static_cast< const float * >( &value.c0.x ) ); }
            virtual crimild::Bool encode( std::string key, const Quaternion &value ) override { return encodeValues( key, 4, static_cast< const float * >( &value.getRawData().x ) ); }
            virtual crimild::Bool encode( std::string key, const Transformation &value ) override;
            virtual crimild::Bool encode( std::string key, const Format &value ) override { return encodeValue( key, value ); }

            virtual bool encode( std::string_view key, std::vector< std::byte > &value ) override
            {
                // TODO
                return false;
            }

            virtual crimild::Bool encode( std::string key, ByteArray &value ) override { return encodeValues( key, value.size(), value.getData() ); }
            virtual crimild::Bool encode( std::string key, Array< crimild::Real32 > &value ) override { return encodeValues( key, value.size(), value.getData() ); }
            virtual crimild::Bool encode( std::string key, Array< Vector3f > &value ) override { return encodeValues( key, 3 * value.size(), value.size() > 0 ? &value[ 0 ].x : nullptr ); }
            virtual crimild::Bool encode( std::string key, Array< Vector4f > &value ) override { return encodeValues( key, 4 * value.size(), value.size() > 0 ? &value[ 0 ].x : nullptr ); }
            virtual crimild::Bool encode( std::string key, Array< Matrix3f > &value ) override { return encodeValues( key, 9 * value.size(), value.size() > 0 ? &value[ 0 ].c0.x : nullptr ); }
            virtual crimild::Bool encode( std::string key, Array< Matrix4f > &value ) override { return encodeValues( key, 16 * value.size(), value.size() > 0 ? &value[ 0 ].c0.x : nullptr ); }
            virtual crimild::Bool encode( std::string key, Array< Quaternion > &value ) override { return false; }

            nlohmann::json getResult( void ) const;

            void write( std::string path ) const;

        protected:
            virtual void encodeArrayBegin( std::string key, crimild::Size count ) override;
            virtual std::string beginEncodingArrayElement( std::string key, crimild::Size index ) override;
            virtual void endEncodingArrayElement( std::string key, crimild::Size index ) override;
            virtual void encodeArrayEnd( std::string key ) override;

        private:
            template< typename T >
            crimild::Bool encodeValue( std::string key, const T &value )
            {
                if ( m_parent != nullptr ) {
                    m_encoded[ m_parent->getUniqueID() ][ key ] = value;
                }
                return true;
            }

            template< typename T >
            crimild::Bool encodeValues( std::string key, crimild::Size count, const T *values )
            {
                if ( m_parent != nullptr ) {
                    m_encoded[ m_parent->getUniqueID() ][ key ] = {};
                    for ( auto i = 0l; i < count; ++i ) {
                        m_encoded[ m_parent->getUniqueID() ][ key ].push_back( values[ i ] );
                    }
                }

                return true;
            }

        private:
            Stack< SharedPointer< Codable > > m_sortedObjects;
            Map< Codable::UniqueID, Map< std::string, Codable::UniqueID > > m_links;
            Map< Codable::UniqueID, nlohmann::json > m_encoded;
            SharedPointer< Codable > m_parent;
            Array< SharedPointer< Codable > > m_roots;
        };

    }

}

#endif
