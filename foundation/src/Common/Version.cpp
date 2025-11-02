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

#include "Common/Version.hpp"

#include "Common/Macros.hpp"
#include "Common/StringUtils.hpp"
#include "Log/Log.hpp"

using namespace crimild;

Version::Version( void )
    : Version( CRIMILD_VERSION_MAJOR, CRIMILD_VERSION_MINOR, CRIMILD_VERSION_PATCH )
{
}

Version::Version( const Version &other )
    : Version( other._major, other._minor, other._patch )
{
}

Version::Version( int major, int minor, int patch )
    : _major( major ),
      _minor( minor ),
      _patch( patch )
{
}

Version::Version( std::string versionStr )
{
    fromString( versionStr );
}

Version::~Version( void )
{
}

Version &Version::operator=( const Version &other )
{
    _major = other._major;
    _minor = other._minor;
    _patch = other._patch;

    return *this;
}

std::string Version::getDescription( void ) const
{
    std::stringstream str;
    str << "v" << getMajor()
        << "." << getMinor()
        << "." << getPatch();

    return str.str();
}

std::string Version::getFullDescription( void ) const
{
    std::stringstream str;
    str << "Crimild Engine"
        << " v" << getMajor() << "." << getMinor() << "." << getPatch()
        << " (c) 2002-present H. Hernán Saez.";
    return str.str();
}

void Version::fromString( std::string str )
{
    _major = 0;
    _minor = 0;
    _patch = 0;

    const std::string VERSION_TAG( "v" );

    auto pos = str.find( VERSION_TAG );
    if ( pos == std::string::npos ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid version string" );
        return;
    }

    auto values = StringUtils::split< int >( str.substr( pos + VERSION_TAG.length() ), '.' );
    if ( values.size() != 3 ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Invalid version string" );
        return;
    }

    _major = values[ 0 ];
    _minor = values[ 1 ];
    _patch = values[ 2 ];
}
