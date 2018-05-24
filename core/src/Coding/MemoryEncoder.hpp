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

#ifndef CRIMILD_CORE_CODING_MEMORY_ENCODER_
#define CRIMILD_CORE_CODING_MEMORY_ENCODER_

#include "Codable.hpp"
#include "Encoder.hpp"
#include "EncodedData.hpp"

#include "Foundation/Containers/Stack.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Mathematics/Transformation.hpp"

namespace crimild {

	namespace coding {

        class MemoryEncoder : public Encoder {
        public:
            MemoryEncoder( void );
            virtual ~MemoryEncoder( void );
            
            virtual void encode( SharedPointer< Codable > const &obj ) override;
            virtual void encode( std::string key, SharedPointer< Codable > const &obj ) override;
            
            virtual void encode( std::string key, std::string value ) override;
            
            virtual void encode( std::string key, const Transformation &value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, crimild::Size value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, crimild::UInt8 value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, crimild::UInt16 value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, crimild::Int16 value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, crimild::Int32 value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, crimild::UInt32 value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, crimild::Real32 value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, crimild::Real64 value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, const Vector2f &value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, const Vector3f &value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, const Vector4f &value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, const Matrix3f &value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, const Matrix4f &value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, const Quaternion4f &value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, crimild::Bool value ) override { encodeData( key, value ); }
            virtual void encode( std::string key, const crimild::VertexFormat &value ) override { encodeData( key, value ); }

			virtual void encode( std::string key, containers::ByteArray &value ) override { encodeData( key, value ); }
			virtual void encode( std::string key, containers::Array< crimild::Real32 > &value ) override { encodeData( key, value ); }
			virtual void encode( std::string key, containers::Array< Vector3f > &value ) override { encodeData( key, value ); }
			virtual void encode( std::string key, containers::Array< Vector4f > &value ) override { encodeData( key, value ); }
			virtual void encode( std::string key, containers::Array< Matrix3f > &value ) override { encodeData( key, value ); }
			virtual void encode( std::string key, containers::Array< Matrix4f > &value ) override { encodeData( key, value ); }
			virtual void encode( std::string key, containers::Array< Quaternion4f > &value ) override { encodeData( key, value ); }
            
            containers::ByteArray getBytes( void ) const;
            
        protected:
			virtual void encodeArrayBegin( std::string key, crimild::Size count ) override;
			virtual std::string beginEncodingArrayElement( std::string key, crimild::Size index ) override;
			virtual void endEncodingArrayElement( std::string key, crimild::Size index ) override;
			virtual void encodeArrayEnd( std::string key ) override;

        private:
            template< typename T >
            void encodeData( std::string key, const T &value )
            {
                auto encoded = crimild::alloc< EncodedData >( value );
                encode( key, crimild::cast_ptr< EncodedData >( encoded ) );
            }
            
            static void append( containers::ByteArray &out, crimild::Int8 value );
            static void append( containers::ByteArray &out, std::string value );
            static void append( containers::ByteArray &out, Codable::UniqueID value );
            static void append( containers::ByteArray &out, const containers::ByteArray &data );
            static void appendRawBytes( containers::ByteArray &out, crimild::Size count, const void *data );
            
        private:
            containers::Stack< SharedPointer< Codable >> _sortedObjects;
            containers::Map< Codable::UniqueID, containers::Map< std::string, Codable::UniqueID >> _links;
            SharedPointer< Codable > _parent;
            containers::Array< SharedPointer< Codable >> _roots;
            
        public:
            virtual std::string dump( void ) override;
        };
        
	}
    
}

#endif

