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

#ifndef CRIMILD_RENDERING_IMAGE_
#define CRIMILD_RENDERING_IMAGE_

#include <memory>

namespace Crimild {

	class Image {
	public:
		Image( void );
		Image( int width, int height, int bpp, const unsigned char *data );
		virtual ~Image( void );

		int getWidth( void ) const { return _width; }
		int getHeight( void ) const { return _height; }
		int getBpp( void ) const { return _bpp; }
		unsigned char *getData( void ) { return _data; }
		const unsigned char *getData( void ) const { return _data; }

		void setData( int width, int height, int bpp, const unsigned char *data );

		bool isLoaded( void ) const { return _data != nullptr; }
		virtual void load( void );
		virtual void unload( void );

	private:
		int _width;
		int _height;
		int _bpp;
		unsigned char *_data;

	private:
		Image( const Image & ) { }
		Image &operator=( const Image & ) { return *this; }
	};

	typedef std::shared_ptr< Image > ImagePtr;
}

#endif

