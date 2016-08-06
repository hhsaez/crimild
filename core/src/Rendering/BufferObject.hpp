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
#include "Foundation/Stream.hpp"

#include <memory>
#include <cstring>
#include <vector>

namespace crimild {

	template< typename T >
	class BufferObject : public StreamObject {
	protected:
		BufferObject( size_t size, const T *data )
		{
			if ( size > 0 ) {
				_data.resize( size );
				if ( data != nullptr ) {
					memcpy( &_data[ 0 ], data, sizeof( T ) * size );
				}
				else {
					memset( &_data[ 0 ], 0, sizeof( T ) * size );					
				}
			}
		}

	public:
		virtual ~BufferObject( void )
		{

		}

		size_t getSize( void ) const { return _data.size(); }
        
        size_t getSizeInBytes( void ) const { return sizeof( T ) * getSize(); }

		T *data( void ) { return &_data[ 0 ]; }

		const T *getData( void ) const { return &_data[ 0 ]; }

	private:
		std::vector< T > _data;

	public:
		BufferObject( void ) { }

		virtual bool registerInStream( Stream &s ) override
		{
			return StreamObject::registerInStream( s );
		}

		virtual void save( Stream &s ) override
		{
			StreamObject::save( s );

			size_t size = getSize();
			s.write( size );
			if ( size > 0 ) {
				s.writeRawBytes( &_data[ 0 ], getSizeInBytes() );
			}
		}

		virtual void load( Stream &s ) override
		{
			StreamObject::load( s );

			size_t size;
			s.read( size );

			if ( size > 0 ) {
				_data.resize( size );
				s.readRawBytes( &_data[ 0 ], getSizeInBytes() );
			}
		}
	};

}

#endif

