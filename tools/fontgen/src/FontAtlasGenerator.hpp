/**
 * Copyright (c) 2013, Hugo Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_TOOLS_FONTGEN_FONT_ATLAS_GENERATOR_
#define CRIMILD_TOOLS_FONTGEN_FONT_ATLAS_GENERATOR_

#include <Crimild.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace crimild {

	namespace tools {

		namespace fontgen {

			class FontAtlasGenerator {
			public:
				FontAtlasGenerator( std::string fontFace, unsigned int width, unsigned int height );
				virtual ~FontAtlasGenerator( void );

				bool execute( std::string output );

			private:
				bool init( void );
				bool loadFont( std::string fontFace );
				void cleanup( void );
				void pushChar( unsigned char c );
				void writeBuffer( FT_GlyphSlot &slot, int x, int y );
				void saveTexture( std::string fileName, bool greyscale = true );
				void saveGlyphs( std::string filename );

				std::vector< unsigned char > _buffer;
				unsigned int _width;
				unsigned int _height;
				unsigned int _pixelSize;
				unsigned int _cursorX;
				unsigned int _cursorY;

				std::ofstream _glyphs;

				FT_Library _library;
				FT_Face _face;

			};

		}

	}

}

#endif

