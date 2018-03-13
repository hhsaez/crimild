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

#include "Foundation/SharedObject.hpp"
#include "Foundation/Memory.hpp"
#include "Foundation/Types.hpp"
#include "Foundation/Containers/Array.hpp"
#include "Rendering/VertexFormat.hpp"
#include "Mathematics/Transformation.hpp"

#include <sstream>

namespace crimild {

	namespace coding {

		class Codable;

        class Encoder : public SharedObject {
        protected:
            Encoder( void );
            
        public:
            virtual ~Encoder( void );
            
            // objects
            virtual void encode( SharedPointer< Codable > const &codable ) = 0;
            virtual void encode( std::string key, SharedPointer< Codable > const &codable ) = 0;
            
            // values
            virtual void encode( std::string key, std::string str ) = 0;
            virtual void encode( std::string key, crimild::Size value ) = 0;
            virtual void encode( std::string key, crimild::UInt16 value ) = 0;
            virtual void encode( std::string key, crimild::Int32 value ) = 0;
            virtual void encode( std::string key, crimild::UInt32 value ) = 0;
            virtual void encode( std::string key, crimild::Bool value ) = 0;
			virtual void encode( std::string key, crimild::Real32 value ) = 0;
			virtual void encode( std::string key, crimild::Real64 value ) = 0;
            virtual void encode( std::string key, const Vector3f & ) = 0;
            virtual void encode( std::string key, const Vector4f & ) = 0;
            virtual void encode( std::string key, const Transformation & ) = 0;
            virtual void encode( std::string key, const VertexFormat & ) = 0;
            
            template< typename T >
            void encode( std::string key, containers::Array< T > &a )
            {
                crimild::Size N = a.size();
                encodeArrayBegin( key, N );
                
                a.each( [ this, key ]( T &elem, crimild::Size i ) {
                    std::stringstream ss;
                    ss << key << "_" << i;
                    encode( ss.str(), elem );
                });
                
                encodeArrayEnd( key );
            }
            
        protected:
            virtual void encodeArrayBegin( std::string key, crimild::Size count ) = 0;
            virtual void encodeArrayEnd( std::string key ) = 0;
            
        public:
            virtual void dump( void );
        };
        
	}
    
}

#endif

