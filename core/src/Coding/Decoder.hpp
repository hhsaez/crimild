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

#include "Foundation/SharedObject.hpp"
#include "Foundation/Memory.hpp"
#include "Foundation/Types.hpp"
#include "Foundation/Containers/Array.hpp"

#include "Mathematics/Transformation.hpp"

namespace crimild {

	namespace coding {

        class Decoder : public SharedObject {
        protected:
            Decoder( void );
        
        public:
            virtual ~Decoder( void );
            
            virtual void decode( std::string key, SharedPointer< coding::Codable > &codable ) = 0;

            virtual void decode( std::string key, std::string &value ) = 0;
            virtual void decode( std::string key, crimild::Size &value ) = 0;
            virtual void decode( std::string key, crimild::Int32 &value ) = 0;
            virtual void decode( std::string key, crimild::Bool &value ) = 0;
			virtual void decode( std::string key, crimild::Real32 &value ) = 0;
			virtual void decode( std::string key, crimild::Real64 &value ) = 0;
			virtual void decode( std::string key, crimild::Vector3f &value ) = 0;
            virtual void decode( std::string key, Transformation &value ) = 0;

            template< typename T >
            void decode( std::string key, containers::Array< SharedPointer< T >> &value )
            {
                auto count = beginDecodingArray( key );
                
                value.resize( count );
                for ( crimild::Size i = 0; i < count; i++ ) {
                    auto v = SharedPointer< Codable >();
                    auto itemKey = beginDecodingArrayElement( key, i );
                    decode( itemKey, v );
                    endDecodingArrayElement( key, i );
                    value[ i ] = crimild::cast_ptr< T >( v );
                }
                
                endDecodingArray( key );
            }
            
			template< typename T >
			void decode( std::string key, containers::Array< T > &value )
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
			void addRootObject( SharedPointer< SharedObject > const &obj );

		private:
			containers::Array< SharedPointer< SharedObject >> _roots;
        };
        
	}
    
}

#endif

