/*
 * Copyright (c) 2015, Hernan Saez
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

#include "Chunk.hpp"
#include "Log.hpp"

#include <cassert>

using namespace crimild;
using namespace crimild::internal;

bool Chunk::init( std::size_t blockSize, unsigned char blocks )
{
	assert( blockSize > 0 );
	assert( blocks > 0 );

	const std::size_t allocSize = blockSize * blocks;
	assert( allocSize / blockSize == blocks );

	data = new unsigned char[ blockSize * blocks ];
	if ( data == nullptr ) {
		return false;
	}

	reset( blockSize, blocks );
	return true;
}

void Chunk::reset( std::size_t blockSize, unsigned char blocks )
{
	assert( blockSize > 0 );
	assert( blocks > 0 );
	assert( ( blockSize * blocks ) / blockSize == blocks );

	firstAvailableBlock = 0;
	blocksAvailable = blocks;

	unsigned char i = 0;
	for ( unsigned char *p = data; i != blocks; p += blockSize ) {
		*p = ++i;
	}
}

void Chunk::release( void )
{
	assert( data != nullptr );
	delete [] data;
	data = nullptr;
}

void *Chunk::allocate( std::size_t blockSize )
{
	if ( isFilled() ) {
		return nullptr;
	}

	// alignment test
	assert( ( firstAvailableBlock * blockSize ) / blockSize == firstAvailableBlock );

	unsigned char *result = data + ( firstAvailableBlock * blockSize );
	
	firstAvailableBlock = *result;
	--blocksAvailable;

	return result;
}

void Chunk::deallocate( void *p, std::size_t blockSize )
{
	assert( p >= data );

	unsigned char *toRelease = static_cast< unsigned char * >( p );

	assert( ( toRelease - data ) % blockSize == 0 );

	unsigned char index = static_cast< unsigned char >( ( toRelease - data ) / blockSize );

	*toRelease = firstAvailableBlock;
	firstAvailableBlock = index;

	assert( firstAvailableBlock == ( toRelease - data ) / blockSize );

	++blocksAvailable;
}

