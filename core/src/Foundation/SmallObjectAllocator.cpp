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

#include "SmallObjectAllocator.hpp"
#include "Log.hpp"

#include <cassert>

using namespace crimild;
using namespace crimild::internal;

std::size_t SmallObjectAllocator::getOffset( std::size_t numBytes, std::size_t alignment )
{
    const std::size_t alignExtra = alignment - 1;
    return ( numBytes + alignExtra ) / alignment;
}

void *SmallObjectAllocator::defaultAlloc( std::size_t numBytes )
{
    Log::debug( CRIMILD_CURRENT_CLASS_NAME, "Allocating ", numBytes, " bytes using default allocator" );
    return ::operator new( numBytes );
}

void SmallObjectAllocator::defaultDealloc( void *p )
{
    ::operator delete( p );
}

SmallObjectAllocator::SmallObjectAllocator( std::size_t pageSize, std::size_t maxObjectSize, std::size_t objectAlignSize )
	: _pool( nullptr ),
	  _maxObjectSize( maxObjectSize ),
	  _objectAlignSize( objectAlignSize )
{
	assert( 0 != _objectAlignSize );

	const std::size_t allocCount = getOffset( _maxObjectSize, _objectAlignSize );
	assert( allocCount > 0 );
	_pool = new FixedAllocator[ allocCount ];
	for ( std::size_t i = 0; i < allocCount; i++ ) {
		_pool[ i ].init( ( i + 1 ) * _objectAlignSize, pageSize );
	}
}

SmallObjectAllocator::~SmallObjectAllocator( void )
{
	if ( _pool != nullptr ) {
		delete [] _pool;
		_pool = nullptr;
	}
}

bool SmallObjectAllocator::trimExcessMemory( void )
{
	bool found = false;

	const std::size_t allocCount = getOffset( getMaxObjectSize(), getAlignment() );

	for ( std::size_t i = 0; i < allocCount; i++ ) {
		if ( _pool[ i ].trimEmptyChunk() ) {
			found = true;
		}
	}

	for ( std::size_t i = 0; i < allocCount; i++ ) {
		if ( _pool[ i ].trimChunkList() ) {
			found = true;
		}
	}

	return found;
}

void *SmallObjectAllocator::allocate( std::size_t numBytes )
{
	if ( numBytes > getMaxObjectSize() ) {
		return defaultAlloc( numBytes );
	}

	assert( _pool != nullptr );

	if ( numBytes == 0 ) numBytes = 1;

	const std::size_t index = getOffset( numBytes, getAlignment() ) - 1;
	const std::size_t allocCount = getOffset( getMaxObjectSize(), getAlignment() );
	assert( index < allocCount );

	FixedAllocator &allocator = _pool[ index ];
	assert( allocator.getBlockSize() >= numBytes );
	assert( allocator.getBlockSize() < numBytes + getAlignment() );
	void *place = allocator.allocate();

	if ( ( place == nullptr ) && trimExcessMemory() ) {
		place = allocator.allocate();
	}

	// shall we throw on error?
	assert( place != nullptr );

	return place;
}

void SmallObjectAllocator::deallocate( void *p, std::size_t size )
{
	if ( p == nullptr ) {
		return;
	}

	assert( _pool != nullptr );

	FixedAllocator *allocator = nullptr;
	const std::size_t allocCount = getOffset( getMaxObjectSize(), getAlignment() );

	Chunk *chunk = nullptr;

	for ( std::size_t i = 0; i < allocCount; i++ ) {
		chunk = _pool[ i ].hasBlock( p );
		if ( chunk != nullptr ) {
			allocator = &_pool[ i ];
			break;
		}
	}

	if ( allocator == nullptr ) {
		defaultDealloc( p );
		return;
	}

	assert( chunk != nullptr );
	const bool found = allocator->deallocate( p, chunk );
	assert( found );
}


