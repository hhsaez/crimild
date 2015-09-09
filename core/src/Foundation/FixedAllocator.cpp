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

#include "FixedAllocator.hpp"
#include "Log.hpp"

#include <cassert>

using namespace crimild;
using namespace crimild::internal;

FixedAllocator::FixedAllocator( void )
	: _blockSize( 0 ),
	  _numBlocks( 0 ),
	  _chunks( 0 ),
	  _allocChunk( nullptr ),
	  _deallocChunk( nullptr ),
	  _emptyChunk( nullptr )
{

}

FixedAllocator::~FixedAllocator( void )
{
	for ( auto i = _chunks.begin(); i != _chunks.end(); i++ ) {
		i->release();
	}
}

void FixedAllocator::init( std::size_t blockSize, std::size_t pageSize )
{
	assert( blockSize > 0 );
	assert( pageSize >= blockSize );

	_blockSize = blockSize;

	std::size_t numBlocks = pageSize / blockSize;

	if ( numBlocks > MAX_OBJECTS_PER_CHUNK ) numBlocks = MAX_OBJECTS_PER_CHUNK;
	else if ( numBlocks < MIN_OBJECTS_PER_CHUNK ) numBlocks = MIN_OBJECTS_PER_CHUNK;

	_numBlocks = static_cast< unsigned char >( numBlocks );
	assert( _numBlocks == numBlocks );
}

std::size_t FixedAllocator::countEmptyChunks( void ) const
{
	return ( _emptyChunk == nullptr ) ? 0 : 1;
}

bool FixedAllocator::makeNewChunk( void )
{
	bool allocated = false;
	
	std::size_t size = _chunks.size();
	if ( _chunks.capacity() == size ) {
		if ( size == 0 ) size = 4;
		_chunks.reserve( size * 2 );
	}

	Chunk newChunk;
	allocated = newChunk.init( _blockSize, _numBlocks );
	if ( allocated ) {
		_chunks.push_back( newChunk );
	}

	if ( !allocated ) {
		return false;
	}

	_allocChunk = &_chunks.back();
	_deallocChunk = &_chunks.front();
    
    Log::Debug << "Created new chunk with " << _blockSize << " bytes" << Log::End;
    
	return true;
}

void *FixedAllocator::allocate( void )
{
	assert( ( _emptyChunk == nullptr ) || ( _emptyChunk->hasAvailable( _numBlocks ) ) );
	assert( countEmptyChunks() < 2 );

	if ( _allocChunk == nullptr || _allocChunk->isFilled() ) {
		if ( _emptyChunk != nullptr ) {
			_allocChunk = _emptyChunk;
			_emptyChunk = nullptr;
		}
		else {
			for ( auto i = _chunks.begin(); ; i++ ) {
				if ( _chunks.end() == i ) {
					if ( !makeNewChunk() ) {
						return nullptr;
					}
					break;
				}

				if ( !i->isFilled() ) {
					_allocChunk = &*i;
					break;
				}
			}
		}
	}
	else if ( _allocChunk == _emptyChunk ) {
		_emptyChunk = nullptr;
	}

	assert( _allocChunk != nullptr );
	assert( !_allocChunk->isFilled() );

	void *place = _allocChunk->allocate( _blockSize );

	assert( ( _emptyChunk == nullptr ) || ( _emptyChunk->hasAvailable( _numBlocks ) ) );
	assert( countEmptyChunks() < 2 );

	return place;
}

bool FixedAllocator::deallocate( void *p, Chunk *hint )
{
	assert( !_chunks.empty() );
	assert( &_chunks.front() <= _deallocChunk );
	assert( &_chunks.back() >= _deallocChunk );
	assert( &_chunks.front() <= _allocChunk );
	assert( &_chunks.back() >= _allocChunk );
	assert( countEmptyChunks() < 2 );

	Chunk *foundChunk = nullptr;
	const std::size_t chunkLength = _numBlocks * _blockSize;

	if ( ( hint != nullptr ) && ( hint->hasBlock( p, chunkLength ) ) ) {
		foundChunk = hint;
	}
	else if ( _deallocChunk->hasBlock( p, chunkLength ) ) {
		foundChunk = _deallocChunk;
	}
	else if ( _allocChunk->hasBlock( p, chunkLength ) ) {
		foundChunk = _allocChunk;
	}
	else {
		foundChunk = vicinityFind( p );
	}

	if ( foundChunk == nullptr ) {
		return false;
	}

	assert( foundChunk->hasBlock( p, chunkLength ) );

	_deallocChunk = foundChunk;

	doDeallocate( p );
	assert( countEmptyChunks() < 2 );

	return true;
}

Chunk *FixedAllocator::vicinityFind( void *p ) const
{
	if ( _chunks.empty() ) {
		return nullptr;
	}

	assert( _deallocChunk != nullptr );

	const std::size_t chunkLength = _numBlocks * _blockSize;

	Chunk *lo = _deallocChunk;
	Chunk *hi = _deallocChunk + 1;

	const Chunk *loBound = &_chunks.front();
	const Chunk *hiBound = &_chunks.back() + 1;

	if ( hi == hiBound ) {
		hi = nullptr;
	}

	while ( true ) {
		if ( lo != nullptr ) {
			if ( lo->hasBlock( p, chunkLength ) ) {
				return lo;
			}

			if ( lo == loBound ) {
				lo = nullptr;
				if ( hi == nullptr ) {
					break;
				}
			}
			else --lo;
		}

		if ( hi != nullptr ) {
			if ( hi->hasBlock( p, chunkLength ) ) {
				return hi;
			}

			if ( ++hi == hiBound ) {
				hi = nullptr; 
				if ( lo == nullptr ) {
					break;
				}
			}
		}
	}

	return nullptr;
}

void FixedAllocator::doDeallocate( void *p )
{
	assert( _deallocChunk->hasBlock( p, _numBlocks * _blockSize ) );
	assert( _emptyChunk != _deallocChunk );
	assert( !_deallocChunk->hasAvailable( _numBlocks ) );
	assert( ( _emptyChunk == nullptr ) || ( _emptyChunk->hasAvailable( _numBlocks ) ) );

	_deallocChunk->deallocate( p, _blockSize );

	if ( _deallocChunk->hasAvailable( _numBlocks ) ) {
		assert( _emptyChunk != _deallocChunk );

		if ( _emptyChunk != nullptr ) {
			Chunk *lastChunk = &_chunks.back();
			if ( _deallocChunk == lastChunk ) {
				_deallocChunk = _emptyChunk;
			}
			else if ( lastChunk != _emptyChunk ) {
				std::swap( *_emptyChunk, *lastChunk );
			}
			assert( lastChunk->hasAvailable( _numBlocks ) );
			lastChunk->release();
			_chunks.pop_back();
			if ( ( _allocChunk == lastChunk ) || _allocChunk->isFilled() ) {
				_allocChunk = _deallocChunk;
			}
		}
		_emptyChunk = _deallocChunk;
	}

	assert( ( _emptyChunk == nullptr ) || ( _emptyChunk->hasAvailable( _numBlocks ) ) );
}

bool FixedAllocator::trimEmptyChunk( void )
{
	assert( ( _emptyChunk == nullptr ) || ( _emptyChunk->hasAvailable( _numBlocks ) ) );

	if ( _emptyChunk == nullptr ) {
		return false;
	}

	assert( !_chunks.empty() );
	assert( countEmptyChunks() == 1 );

	Chunk *lastChunk = &_chunks.back();
	if ( lastChunk != _emptyChunk ) {
		std::swap( *_emptyChunk, *lastChunk );
	}

	assert( lastChunk->hasAvailable( _numBlocks ) );

	lastChunk->release();
	_chunks.pop_back();

	if ( _chunks.empty() ) {
		_allocChunk = nullptr;
		_deallocChunk = nullptr;
	}
	else {
		if ( _deallocChunk == _emptyChunk ) {
			_deallocChunk = &_chunks.front();
			assert( _deallocChunk->blocksAvailable < _numBlocks );
		}
	    
		if ( _allocChunk == _emptyChunk ) {
			_allocChunk = &_chunks.back();
			assert( _allocChunk->blocksAvailable < _numBlocks );
		}
	}

	_emptyChunk = nullptr;
	assert( countEmptyChunks() == 0 );

	return true;
}

bool FixedAllocator::trimChunkList( void )
{
	if ( _chunks.empty() ) {
		assert( _allocChunk == nullptr );
		assert( _deallocChunk == nullptr );
	}

	if ( _chunks.size() == _chunks.capacity() ) {
		return false;
	}

	{
		// dirty trick to remove excess capacity
		ChunkArray temp( _chunks );
		temp.swap( _chunks );
	}

	if ( _chunks.empty() ) {
		_deallocChunk = nullptr;
		_allocChunk = nullptr;
	}
	else {
		_deallocChunk = &_chunks.front();
		_allocChunk = &_chunks.back();
	}

	return true;
}

Chunk *FixedAllocator::hasBlock( void *p )
{
	const std::size_t chunkLength = _numBlocks * _blockSize;
	for ( auto it = _chunks.begin(); it != _chunks.end(); it++ ) {
		Chunk &chunk = *it;
		if ( chunk.hasBlock( p, chunkLength ) ) {
			return &chunk;
		}
	}
	return nullptr;
}


