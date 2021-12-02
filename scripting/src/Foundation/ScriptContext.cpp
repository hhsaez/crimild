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

#include "ScriptContext.hpp"

#include <Simulation/FileSystem.hpp>
#include <Simulation/Simulation.hpp>
#include <Simulation/Settings.hpp>

#include <sstream>

using namespace crimild;
using namespace crimild::scripting;

ScriptEvaluator::ScriptEvaluator( void )
    : ScriptEvaluator( nullptr, "" )
{
    
}

ScriptEvaluator::ScriptEvaluator( ScriptContext *ctx, std::string prefix )
    : _context( ctx ),
      _prefix( prefix )
{
    
}

ScriptEvaluator::ScriptEvaluator( const ScriptEvaluator &other )
    : _context( other._context ),
      _prefix( other._prefix )
{
    
}

ScriptEvaluator::~ScriptEvaluator( void )
{
    
}

ScriptEvaluator &ScriptEvaluator::operator=( const crimild::scripting::ScriptEvaluator &other )
{
    _context = other._context;
    _prefix = other._prefix;
    return *this;
}

bool ScriptEvaluator::foreach( const std::string &name, std::function< void( ScriptEvaluator &, int )> callback )
{
    return getContext()->foreach( expandExpression( name ), callback );
}

ScriptContext::ScriptContext( void )
	: ScriptContext( true )
{
	reset();
}

ScriptContext::ScriptContext( bool openDefaultLibs )
	: _state( nullptr ),
	  _openDefaultLibs( openDefaultLibs ),
      _evaluator( this ),
	  _backgroundThreadState( nullptr )
{
	reset();
}

ScriptContext::~ScriptContext( void )
{
	if ( _state != nullptr ) {
		lua_close( _state );
		_state = nullptr;
	}
}

void ScriptContext::reset( void )
{
	if ( _state != nullptr ) {
		lua_close( _state );
		_state = nullptr;
	}

	_state = luaL_newstate();
	if ( _openDefaultLibs ) {
		luaL_openlibs( _state );
	}

	_backgroundThreadState = nullptr;

	// this trick makes sure the runtime directory is in the search
	// path for external modules
	auto baseDir = FileSystem::getInstance().getBaseDirectory();
	baseDir = StringUtils::replaceAll(baseDir, "\\", "/");

    std::stringstream ss;
    ss << "package.path = '"
        << baseDir << "/?.lua;"
        << baseDir << "/assets/?.lua;"
        << baseDir << "/assets/scripts/?.lua;";

    if ( auto settings = Simulation::getInstance()->getSettings() ) {
        auto searchDirs = settings->get< std::string >( "scripting.additional_search_dirs", "" );
        if ( searchDirs != "" ) {
            auto dirs = StringUtils::split< std::string >( searchDirs, ';' );
            for ( auto d : dirs ) {
                if ( d.length() == 0 ) continue;
                ss << baseDir << "/" << d << "/?.lua;";
            }
        }
    }

    ss << "package.path'";
    parse( ss.str() );
}

bool ScriptContext::load( std::string fileName, bool supportCoroutines )
{
	if ( supportCoroutines ) {
		// TODO: I think this should go in the constructor...
		_backgroundThreadState = lua_newthread( _state );
	}
	else {
		_backgroundThreadState = nullptr;
	}

	if ( luaL_dofile( _state, fileName.c_str() ) ) {
        std::string reason = lua_tostring( _state, -1 );
#if CRIMILD_SCRIPTING_LOG_VERBOSE
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot execute script in file ", fileName, "\n\tReason: ", reason );
		parse( "debug.traceback()" ); // prints stack trace
#endif
	    return false;
	}

	return true;
}

bool ScriptContext::parse( std::string text )
{
	if ( luaL_dostring( _state, text.c_str() ) && failOnParse() ) {
        std::string reason = lua_tostring( _state, -1 );
#if CRIMILD_SCRIPTING_LOG_VERBOSE
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot parse string \"", text, "\"", "\n\tReason: ", reason );
		parse( "debug.traceback()" ); // prints stack trace
#endif
	    return false;
	}

	return true;
}

int ScriptContext::yield( void )
{
	if ( _backgroundThreadState == nullptr ) {
		return 0;
	}

	return lua_yield( _backgroundThreadState, 0 );
}

int ScriptContext::resume( void )
{
	if ( _backgroundThreadState == nullptr ) {
		return 0;
	}

	return lua_resume( _backgroundThreadState, NULL, 0 );
}

std::string ScriptContext::dumpStack( void )
{
	return LuaUtils::dumpStack( _state );
}

bool ScriptContext::foreach( const std::string &name, std::function< void( ScriptEvaluator &, int ) > callback )
{
    int count;
    if ( !getEvaluator().getPropValue( "#" + name, count ) ) {
        return false;
    }
    
	for ( int i = 0; i < count; i++ ) {
        std::stringstream str;
        str << name << "[" << ( i + 1 ) << "]";
        ScriptEvaluator eval( this, str.str() );
        callback( eval, i );
	}
    
    return true;
}

