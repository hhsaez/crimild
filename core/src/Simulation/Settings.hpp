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
#include "Foundation/Memory.hpp"

#include <map>
#include <sstream>
#include <string>

namespace crimild {

    class Settings : public SharedObject {
    public:
        static const char *SETTINGS_APP_NAME;
        static const char *SETTINGS_APP_VERSION_MAJOR;
        static const char *SETTINGS_APP_VERSION_MINOR;
        static const char *SETTINGS_APP_VERSION_PATCH;
        static const char *SETTINGS_RENDERING_SHADOWS_ENABLED;
        static const char *SETTINGS_RENDERING_SHADOWS_RESOLUTION_WIDTH;
        static const char *SETTINGS_RENDERING_SHADOWS_RESOLUTION_HEIGHT;

    public:
        Settings( void );

        Settings( int argc, char **argv );

        virtual ~Settings( void );

        virtual void load( std::string filename ) { }

        virtual void save( std::string filename ) { }

        void set( std::string key, std::string value )
        {
            _settings[ key ] = value;
        }

        void set( std::string key, const char *value )
        {
            set( key, std::string( value ) );
        }

        template< typename T >
        void set( std::string key, T value )
        {
            std::stringstream str;
            str << value;
            _settings[ key ] = str.str();
        }

        bool hasKey( std::string key )
        {
            return ( _settings.find( key ) != _settings.end() );
        }

        std::string get( std::string key, const char *defaultValue )
        {
            if ( _settings.find( key ) == _settings.end() ) {
                // key not found
                return defaultValue;
            }

            return _settings[ key ];
        }

        std::string get( std::string key, std::string defaultValue )
        {
            if ( _settings.find( key ) == _settings.end() ) {
                // key not found
                return defaultValue;
            }

            return _settings[ key ];
        }

        template< typename T >
        T get( std::string key, T defaultValue = T() )
        {
            if ( !hasKey( key ) ) {
                // key not found
                return defaultValue;
            }

            std::stringstream str;
            str << _settings[ key ];
            T value;
            str >> value;
            return value;
        }

        void parseCommandLine( int argc, char **argv );

        void each( std::function< void( std::string, Settings * ) > callback );

    private:
        std::map< std::string, std::string > _settings;
    };

    using SettingsPtr = SharedPointer< Settings >;

}

#endif
