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

#ifndef CRIMILD_SIMULATION_SETTINGS_
#define CRIMILD_SIMULATION_SETTINGS_

#include "Foundation/Log.hpp"

#include <string>
#include <sstream>
#include <map>

namespace crimild {

	class Settings {
	public:
		Settings( void );

		virtual ~Settings( void );

		template< typename T >
		void add( std::string key, T value )
		{
			std::stringstream str;
			str << value;
			_settings[ key ] = str.str();
		}
		
		void add( std::string key, const char *value )
		{
			add( key, std::string( value ) );
		}

		void add( std::string key, std::string value )
		{
			_settings[ key ] = value;
		}

		template< typename T >
		T get( std::string key, T defaultValue )
		{
			if ( _settings.find( key ) == _settings.end() ) {
				// key not found
				return defaultValue;
			}
			
			std::stringstream str;
			str << _settings[ key ];
			T value;
			str >> value;
			return value;
		}

		std::string get( const char *key, const char *defaultValue )
		{
			return get( std::string( key ), std::string( defaultValue ) );
		}

		std::string get( std::string key, std::string defaultValue )
		{
			if ( _settings.find( key ) == _settings.end() ) {
				// key not found
				return defaultValue;
			}
			
			return _settings[ key ];
		}

		void parseCommandLine( int argc, char **argv );

		void dump( void );

	private:
		std::map< std::string, std::string > _settings;
	};

}

#endif


