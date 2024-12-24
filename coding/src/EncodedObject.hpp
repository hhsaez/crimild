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

#ifndef CRIMILD_CORE_CODING_ENCODED_OBJECT_
#define CRIMILD_CORE_CODING_ENCODED_OBJECT_

#include "Codable.hpp"
#include "Crimild_Foundation.hpp"

namespace crimild {

    namespace coding {

        class EncodedObject : public Codable {
            CRIMILD_IMPLEMENT_RTTI( crimild::coding::EncodedObject )
        public:
            EncodedObject( void )
            {
            }

            virtual ~EncodedObject( void )
            {
            }

            void set( std::string key, crimild::Int32 value );

            // void set( std::string key, SharedPointer< EncodedArray > &array );

            void set( std::string key, SharedPointer< EncodedObject > &obj );

            inline void setBytes( const ByteArray &bytes ) { _bytes = bytes; }
            inline ByteArray &getBytes( void ) { return _bytes; }
            inline const ByteArray &getBytes( void ) const { return _bytes; }

            std::string getString( void ) const
            {
                return std::string( ( const char * ) &_bytes[ 0 ] );
            }

            template< typename T >
            T getValue( void ) const
            {
                auto value = T();
                if ( _bytes.size() > 0 ) {
                    memcpy( ( void * ) &value, &_bytes[ 0 ], _bytes.size() );
                }
                return value;
            }

        private:
            ByteArray _bytes;
        };

    }

}

#endif
