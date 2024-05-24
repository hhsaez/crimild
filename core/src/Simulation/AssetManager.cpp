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

#include "AssetManager.hpp"

#include "Crimild_Foundation.hpp"
#include "FileSystem.hpp"
#include "Rendering/Font.hpp"
#include "Rendering/ImageTGA.hpp"
#include "Rendering/SystemFont.hpp"
#include "Rendering/Texture.hpp"
#include "Settings.hpp"
#include "Simulation.hpp"

using namespace crimild;

AssetManager::AssetManager( void )
{
    if ( auto sim = Simulation::getInstance() ) {
        if ( auto settings = sim->getSettings() ) {
            auto systemFont = settings->get< std::string >( "fonts.system", "" );
            if ( systemFont != "" ) {
                loadFont( FONT_SYSTEM, systemFont );
            } else {
                set( FONT_SYSTEM, crimild::alloc< SystemFont >(), true );
            }

            auto defaultFont = settings->get< std::string >( "fonts.default", "" );
            if ( defaultFont != "" ) {
                loadFont( FONT_DEFAULT, defaultFont );
            } else {
                set( FONT_DEFAULT, crimild::retain( get< Font >( FONT_SYSTEM ) ), true );
            }
        }
    }
}

AssetManager::~AssetManager( void )
{
    clear();
}

namespace crimild {

    template<>
    Texture *AssetManager::get< Texture >( std::string name )
    {
        Texture *texture = nullptr;

        {
            ScopedLock lock( _mutex );
            texture = static_cast< Texture * >( crimild::get_ptr( _assets[ name ] ) );
        }

        if ( texture == nullptr && ( StringUtils::getFileExtension( name ) == "tga" ) ) {
            auto image = crimild::alloc< ImageTGA >( FileSystem::getInstance().pathForResource( name ) );
            if ( image != nullptr ) {
                auto tmp = crimild::alloc< Texture >( image );
                set( name, tmp );
                texture = crimild::get_ptr( tmp );
            }
        }

        return texture;
    }

}

void AssetManager::loadFont( std::string name, std::string fileName )
{
    std::string fontDefFileName = FileSystem::getInstance().pathForResource( fileName );
    auto font = crimild::alloc< Font >( fontDefFileName );
    set( name, font, true );
}
