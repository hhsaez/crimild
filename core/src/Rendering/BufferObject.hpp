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

#ifndef CRIMILD_RENDERING_BUFFER_OBJECT_
#define CRIMILD_RENDERING_BUFFER_OBJECT_

#include "Foundation/Macros.hpp"
#include "Foundation/Types.hpp"
#include "Foundation/Containers/Array.hpp"
#include "Streaming/Stream.hpp"
#include "Coding/Codable.hpp"
#include "Coding/Encoder.hpp"
#include "Coding/Decoder.hpp"

#include <memory>
#include <cstring>

namespace crimild {

	template< typename T >
    class BufferObject : public coding::Codable, public StreamObject {
	protected:
		BufferObject( unsigned int size, const T *data )
		{
			if ( size > 0 ) {
                _data.resize( size * sizeof( T ) );
                if ( data != nullptr ) {
                    memcpy( &_data[ 0 ], data, sizeof( T ) * size );
                }
			}
		}

	public:
		virtual ~BufferObject( void )
		{

		}

		inline unsigned int getSize( void ) const { return _data.size() / sizeof( T ); }
        
        inline unsigned int getSizeInBytes( void ) const { return _data.size(); }

		inline T *data( void ) { return ( T * ) &_data[ 0 ]; }

		inline const T *getData( void ) const { return ( const T * ) &_data[ 0 ]; }

		inline crimild::Size getUsedCount( void ) const { return getSize(); }

		inline void setUsedCount( crimild::Size count ) { _data.resize( count * sizeof( T ) ); }

	private:
        containers::ByteArray _data;
        
        /**
            name Coding
         */
        //@{
        
    public:
        virtual void encode( coding::Encoder &encoder ) override
        {
            Codable::encode( encoder );
            
            encoder.encode( "data", _data );
        }
        
        virtual void decode( coding::Decoder &decoder ) override
        {
            Codable::decode( decoder );
            
            decoder.decode( "data", _data );
        }
        
        //@}
        
        /**
            \name Streaming
            \deprecated See crimild::coding
         */
        //@{

	public:
		BufferObject( void ) { }

		virtual bool registerInStream( Stream &s ) override
		{
			return StreamObject::registerInStream( s );
		}

		virtual void save( Stream &s ) override
		{
			StreamObject::save( s );

			unsigned int size = _data.size();
			s.write( size );
            
			s.write( size ); // used count
			
			if ( size > 0 ) {
				s.writeRawBytes( &_data[ 0 ], size );
			}
		}

		virtual void load( Stream &s ) override
		{
			StreamObject::load( s );

			unsigned int size;
			s.read( size );

			if ( s.getVersion() >= Version( 4, 5, 0 ) ) {
				s.read( size );
			}

			if ( size > 0 ) {
				_data.resize( size );
				s.readRawBytes( &_data[ 0 ], size );
			}
		}
        
        //@}
	};

}

#endif

