/*
 * Copyright (c) 2002 - present, H. Hernan Saez
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

#include "Font.hpp"

#include "Crimild_Foundation.hpp"
#include "Simulation/AssetManager.hpp"
#include "Simulation/FileSystem.hpp"

#include <fstream>

using namespace crimild;

Font::Font( void )
{
}

Font::Font( std::string fontDefFile )
{
    loadGlyphs( fontDefFile );
}

Font::~Font( void )
{
}

void Font::loadGlyphs( std::string file )
{
    _glyphs.clear();

    auto fontNamePrefix = file.substr( 0, file.find_last_of( "." ) );
    fontNamePrefix = FileSystem::getInstance().getRelativePath( fontNamePrefix );

    auto textureFileName = fontNamePrefix + ".tga";
    if ( auto texture = AssetManager::getInstance()->get< Texture >( textureFileName ) ) {
        _texture = crimild::retain( texture );
    }

    auto sdfTextureFileName = fontNamePrefix + "_sdf.tga";
    if ( auto sdfTexture = AssetManager::getInstance()->get< Texture >( sdfTextureFileName ) ) {
        _sdfTexture = crimild::retain( sdfTexture );
    }

    std::ifstream input;
    input.open( file );
    if ( !input.is_open() ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot open glyph file ", file );
        return;
    }

    char buffer[ 1024 ];
    while ( !input.eof() ) {
        input.getline( buffer, 1024 );
        std::stringstream line;
        line << buffer;

        Font::Glyph glyph;
        int symbol;
        line >> symbol
            >> glyph.width
            >> glyph.height
            >> glyph.bearingX
            >> glyph.bearingY
            >> glyph.advance
            >> glyph.uOffset
            >> glyph.vOffset
            >> glyph.u
            >> glyph.v;
        glyph.symbol = ( unsigned char ) symbol;
        _glyphs[ glyph.symbol ] = glyph;
    }
}
