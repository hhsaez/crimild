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

#include "FileStream.hpp"
#include "Foundation/Log.hpp"

using namespace crimild;

FileStream::FileStream( std::string path, FileStream::OpenMode openMode )
	: _path( path ),
	  _openMode( openMode ),
	  _file( nullptr )
{

}

FileStream::~FileStream( void )
{
	close();
}

bool FileStream::open( void )
{
	close();

	_file = fopen( _path.c_str(), _openMode == FileStream::OpenMode::WRITE ? "w" : "r" );
	if ( _file == nullptr ) {
		Log::Error << "Invalid file path " << _path;
		return false;
	}

	return true;
}

bool FileStream::close( void )
{
	if ( _file != nullptr ) {
		fclose( _file );
		_file = nullptr;
	}	

	return true;
}

bool FileStream::flush( void )
{
	open();
	auto result = Stream::flush();
	close();

	return result;
}

bool FileStream::load( void )
{
	open();
	auto result = Stream::load();
	close();

	return result;
}

void FileStream::writeRawBytes( const void *bytes, size_t size )
{
	fwrite( bytes, 1, size, _file );
}

void FileStream::readRawBytes( void *bytes, size_t size )
{
	fread( bytes, 1, size, _file );
}

