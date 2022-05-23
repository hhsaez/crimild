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
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "FileSystem.hpp"

#include "Exceptions/FileNotFoundException.hpp"
#include "Foundation/Log.hpp"
#include "Mathematics/Numeric.hpp"

using namespace crimild;

FileSystem &FileSystem::getInstance( void )
{
    static FileSystem fs;
    return fs;
}

FileSystem::FileSystem( void )
{
    setBaseDirectory( "." );
}

FileSystem::~FileSystem( void )
{
}

void FileSystem::setBaseDirectory( std::string baseDirectory )
{
    _baseDirectory = baseDirectory;

    setDocumentsDirectory( _baseDirectory );
}

void FileSystem::init( int argc, char **argv )
{
    std::string base = "";
    if ( argc > 0 ) {
        base = extractDirectory( argv[ 0 ] );
    }

    int pos = base.find( "/Debug" );
    if ( pos > 0 ) {
        base = base.substr( 0, pos );
    }

    pos = base.find( "/Release" );
    if ( pos > 0 ) {
        base = base.substr( 0, pos );
    }

    pos = base.find( "\\Debug" );
    if ( pos > 0 ) {
        base = base.substr( 0, pos );
    }

    pos = base.find( "\\Release" );
    if ( pos > 0 ) {
        base = base.substr( 0, pos );
    }

    pos = base.find( "/Contents/MacOS" );
    if ( pos > 0 ) {
        base = base.substr( 0, pos );
        base = base + "/Contents/Resources";
    }

    if ( base.length() == 0 ) {
        base = ".";
    }

    setBaseDirectory( base );
}

std::string FileSystem::extractDirectory( std::string path )
{
    // handles both Win32 and Unix like paths
    auto dir = path.substr( 0, Numerici::max( path.find_last_of( '\\' ), path.find_last_of( '/' ) ) );
    if ( dir == path ) {
        return "";
    }

    return dir;
}

std::string FileSystem::pathForResource( std::string filePath ) const
{
    return getBaseDirectory() + "/" + getRelativePath( filePath );
}

std::string FileSystem::pathForDocument( std::string filePath ) const
{
    return getDocumentsDirectory() + "/" + filePath;
}

std::string FileSystem::getRelativePath( std::string absolutePath ) const
{
    // check if the absolute path includes the base
    // directory at the very beginning
    int pos = absolutePath.find( _baseDirectory + "/" );
    if ( pos == 0 ) {
        return absolutePath.substr( _baseDirectory.length() + 1 );
    }

    return absolutePath;
}

std::string FileSystem::getFileName( std::string path, bool includeExtension )
{
    auto pos = Numerici::max( path.find_last_of( '\\' ), path.find_last_of( '/' ) );
    if ( pos >= 0 ) {
        path = path.substr( pos + 1 );
    }

    if ( !includeExtension ) {
        pos = path.find_last_of( "." );
        if ( pos >= 0 ) {
            path = path.substr( 0, pos );
        }
    }

    return path;
}

std::vector< std::byte > FileSystem::readResourceFile( std::string relativePath ) const
{
    auto filePath = pathForResource( relativePath );
    return readFile( filePath );
}

std::vector< std::byte > FileSystem::readFile( std::string absolutePath ) const
{
    std::ifstream file( absolutePath, std::ios::ate | std::ios::binary );
    if ( !file.is_open() ) {
        throw FileNotFoundException( "Failed to open file: " + absolutePath );
    }

    auto fileSize = ( size_t ) file.tellg();
    std::vector< std::byte > buffer( fileSize );

    file.seekg( 0 );
    file.read( reinterpret_cast< char * >( buffer.data() ), fileSize );
    file.close();

    CRIMILD_LOG_DEBUG( "File ", absolutePath, " loaded (", fileSize, " bytes)" );

    return buffer;
}
