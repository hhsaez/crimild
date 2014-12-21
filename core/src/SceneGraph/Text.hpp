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

#ifndef CRIMILD_CORE_SCENE_GRAPH_TEXT_
#define CRIMILD_CORE_SCENE_GRAPH_TEXT_

#include "Geometry.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/Material.hpp"

namespace crimild {

	class Font : public SharedObject {
	public:
		struct Glyph {
			char symbol;
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
		Font( std::string faceFilePath, std::string glyphFilePath );
		virtual ~Font( void );

		ImagePtr getFace( void ) { return _face; }
		Glyph getGlyph( char c ) { return _glyphs[ c ]; }

	private:
		void loadGlyphs( std::string file );

		ImagePtr _face;
		std::map< char, Glyph > _glyphs;
	};
    
    using FontPtr = std::shared_ptr< Font >;

	class Text : public Geometry {
	public:
		Text( void );
		virtual ~Text( void );

		std::string getText( void ) const { return _text; }
		void setText( std::string text );

		float getSize( void ) const { return _size; }
		void setSize( float size );

		FontPtr getFont( void ) { return _font; }
		void setFont( FontPtr const &font );

		MaterialPtr getMaterial( void ) { return _material; }

	private:
		void updatePrimitive( void );

		std::string _text;
		float _size;
		FontPtr _font;
		PrimitivePtr _primitive;
		MaterialPtr _material;
	};
    
    using TextPtr = std::shared_ptr< Text >;

}

#endif

