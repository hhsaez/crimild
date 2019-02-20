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

#ifndef CRIMILD_CORE_RENDERING_FONT_
#define CRIMILD_CORE_RENDERING_FONT_

#include "Foundation/SharedObject.hpp"

#include <map>

namespace crimild {

	class Texture;

	class Font : public SharedObject {
	public:
		struct Glyph {
			unsigned char symbol;
			float width;
			float height;
			float bearingX;
			float bearingY;
			float advance;
			float uOffset;
			float vOffset;
			float u;		
			float v;
		};

	public:
		Font( void );
		explicit Font( std::string defFileName );
		virtual ~Font( void );

		Texture *getTexture( void ) { return crimild::get_ptr( _texture ); }
		Texture *getSDFTexture( void ) { return crimild::get_ptr( _sdfTexture ); }

		Glyph getGlyph( unsigned char c ) { return _glyphs[ c ]; }

	private:
		void loadGlyphs( std::string file );

	protected:
		SharedPointer< Texture > _texture;
		SharedPointer< Texture > _sdfTexture;

		std::map< unsigned char, Glyph > _glyphs;
	};
    
}

#endif

