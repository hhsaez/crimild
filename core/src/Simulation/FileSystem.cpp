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

#include "FileSystem.hpp"

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
    
	if ( base.length() == 0 ) {
		base = ".";
	}

	setBaseDirectory( base );
}

std::string FileSystem::extractDirectory( std::string path )
{
	// handles both Win32 and Unix like paths
	return path.substr( 0, Numerici::max( path.find_last_of( '\\' ), path.find_last_of( '/' ) ) );
}

std::string FileSystem::pathForResource( std::string filePath )
{
	return getBaseDirectory() + "/" + filePath;
}

std::string FileSystem::pathForDocument( std::string filePath )
{
	return getDocumentsDirectory() + "/" + filePath;
}

std::string FileSystem::getRelativePath( std::string absolutePath )
{
    int pos = absolutePath.find( _baseDirectory );
    if ( pos >= 0 ) {
        return absolutePath.substr( _baseDirectory.length() );
    }
    
    return absolutePath;
}

