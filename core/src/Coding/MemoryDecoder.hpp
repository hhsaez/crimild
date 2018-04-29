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

#include "Foundation/Version.hpp"
#include "Foundation/Containers/Map.hpp"

namespace crimild {

	namespace coding {

        class MemoryDecoder : public Decoder {
        public:
            MemoryDecoder( void );
            virtual ~MemoryDecoder( void );
            
            virtual void decode( std::string key, SharedPointer< coding::Codable > &codable ) override;

            virtual void decode( std::string key, std::string &value ) override;
            
            virtual void decode( std::string key, crimild::Size &value ) override { decodeData( key, value ); }
            virtual void decode( std::string key, crimild::UInt8 &value ) override { decodeData( key, value ); }
            virtual void decode( std::string key, crimild::UInt16 &value ) override { decodeData( key, value ); }
            virtual void decode( std::string key, crimild::Int16 &value ) override { decodeData( key, value ); }
            virtual void decode( std::string key, crimild::Int32 &value ) override { decodeData( key, value ); }
            virtual void decode( std::string key, crimild::UInt32 &value ) override { decodeData( key, value ); }
            virtual void decode( std::string key, crimild::Bool &value ) override { decodeData( key, value ); }
			virtual void decode( std::string key, crimild::Real32 &value ) override { decodeData( key, value ); }
			virtual void decode( std::string key, crimild::Real64 &value ) override { decodeData( key, value ); }
			virtual void decode( std::string key, crimild::Vector3f &value ) override { decodeData( key, value ); }
            virtual void decode( std::string key, crimild::Vector4f &value ) override { decodeData( key, value ); }
            virtual void decode( std::string key, crimild::Matrix3f &value ) override { decodeData( key, value ); }
            virtual void decode( std::string key, crimild::Matrix4f &value ) override { decodeData( key, value ); }
            virtual void decode( std::string key, crimild::Quaternion4f &value ) override { decodeData( key, value ); }
            virtual void decode( std::string key, Transformation &value ) override { decodeData( key, value ); }
            virtual void decode( std::string key, VertexFormat &value ) override { decodeData( key, value ); }
            
            crimild::Bool fromBytes( const containers::ByteArray &bytes );
            
        private:
            template< typename T >
            void decodeData( std::string key, T &value )
            {
                auto obj = crimild::cast_ptr< EncodedData >( _links[ _currentObj->getUniqueID() ][ key ] );
                value = obj->getValue< T >();
            }

		protected:
			virtual crimild::Size beginDecodingArray( std::string key ) override;
			virtual std::string beginDecodingArrayElement( std::string key, crimild::Size index ) override;
			virtual void endDecodingArrayElement( std::string key, crimild::Size index ) override;
			virtual void endDecodingArray( std::string key ) override;

        private:
            static crimild::Size read( const containers::ByteArray &bytes, Codable::UniqueID &value, crimild::Size offset );
            static crimild::Size read( const containers::ByteArray &bytes, std::string &value, crimild::Size offset );
            static crimild::Size read( const containers::ByteArray &bytes, containers::ByteArray &value, crimild::Size offset );
            static crimild::Size readRawBytes( const containers::ByteArray &bytes, void *data, crimild::Size count, crimild::Size offset );
            
        private:
            containers::Map< Codable::UniqueID, containers::Map< std::string, SharedPointer< Codable >>> _links;
            containers::Map< Codable::UniqueID, SharedPointer< Codable >> _objects;
            SharedPointer< Codable > _currentObj;
            Version _version;
        };
        
	}
    
}

#endif

