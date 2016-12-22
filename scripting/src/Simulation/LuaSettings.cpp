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

#include "LuaSettings.hpp"
#include "Foundation/ScriptContext.hpp"

using namespace crimild;
using namespace crimild::scripting;

LuaSettings::LuaSettings( void )
{
    
}

LuaSettings::LuaSettings( int argc, char **argv, std::string filename )
    : Settings( argc, argv )
{
    set( "video.width", 1024 );
    set( "video.height", 768 );
    set( "video.fullscreen", false );
    
    if ( filename != "" ) {
        load( filename );
    }
}

LuaSettings::~LuaSettings( void )
{
    
}

void LuaSettings::load( std::string filename )
{
    ScriptContext context;
    if ( context.load( FileSystem::getInstance().pathForResource( filename ) ) ) {
        auto &eval = context.getEvaluator();
        each( [&eval]( std::string name, Settings *settings ) {
            std::string value;
            if ( eval.getPropValue( name, value ) && value != "" ) {
                settings->set( name, value );
            }
        });
    }
    else {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot open file ", filename );
    }
}

void LuaSettings::save( std::string filename )
{
    
}

