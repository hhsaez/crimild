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

#include "Foundation/Containers/Stack.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Mathematics/Transformation.hpp"

namespace crimild {

	namespace coding {

        class MemoryEncoder : public Encoder {
        private:
            class EncodedData : public Codable {
                CRIMILD_IMPLEMENT_RTTI( crimild::coding::EncodedData )
            public:
                explicit EncodedData( std::string str )
                    : _dataSize( sizeof( crimild::Char ) * str.length() ),
                      _data( new crimild::Byte[ _dataSize ] )
                {
                    if ( _dataSize > 0 ) {
                        memcpy( &_data[ 0 ], ( const void * ) &str[ 0 ], _dataSize );
                    }
                }
                
                template< typename T >
                explicit EncodedData( const T &data )
                    : _dataSize( sizeof( T ) ),
                      _data( new crimild::Byte[ _dataSize ] )
                {
                    if ( _dataSize > 0 ) {
                        memcpy( &_data[ 0 ], ( const void * ) &data, _dataSize );
                    }
                }
                
                virtual ~EncodedData( void )
                {
                    
                }
                
                template< typename T >
                T getValue( void ) const
                {
                    auto value = T();
                    if ( _dataSize > 0 ) {
                        memcpy( ( void * ) &value, &_data[ 0 ], sizeof( crimild::Byte ) * _dataSize );
                    }
                    return value;
                }
                
            private:
                crimild::Size _dataSize;
                std::unique_ptr< crimild::Byte [] > _data;
            };

        public:
            MemoryEncoder( void );
            virtual ~MemoryEncoder( void );
            
            virtual void encode( SharedPointer< Codable > const &obj ) override;
            virtual void encode( std::string key, SharedPointer< Codable > const &obj ) override;
            
            virtual void encode( std::string key, std::string value ) override;
            virtual void encode( std::string key, const Transformation &value ) override;
            virtual void encode( std::string key, crimild::Size value ) override;
            virtual void encode( std::string key, crimild::Int32 value ) override;
            virtual void encode( std::string key, crimild::Bool value ) override;
            
        protected:
			virtual void encodeArrayBegin( std::string key, crimild::Size count ) override;
			virtual void encodeArrayEnd( std::string key ) override;
            
        private:
            containers::Stack< SharedPointer< Codable >> _sortedObjects;
            containers::Map< Codable::UniqueID, containers::Map< std::string, Codable::UniqueID >> _links;
            SharedPointer< Codable > _parent;
            containers::Array< SharedPointer< Codable >> _roots;

        public:
            virtual void dump( void ) override;
        };
        
	}
    
}

#endif

