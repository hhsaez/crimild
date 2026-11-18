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

#ifndef CRIMILD_MEMORY_ALLOCATOR_
#define CRIMILD_MEMORY_ALLOCATOR_

#include <memory>

namespace crimild {

	namespace internal {

		/**
		   \brief Manages a chunk of memory containing a fixed amount of blocks
		*/
		struct Chunk {
			bool init( std::size_t blockSize, unsigned char blocks );
			void reset( std::size_t blockSize, unsigned char blocks );

			void release( void );

			inline bool hasAvailable( unsigned char numBlocks ) const { return ( blocksAvailable == numBlocks ); }
			inline bool isFilled( void ) const { return blocksAvailable == 0; }

			inline bool hasBlock( void *p, std::size_t chunkLength ) const 
			{
				unsigned char *pc = static_cast< unsigned char * >( p );
				return ( data <= pc ) && ( pc < data + chunkLength );
			}

			void *allocate( std::size_t blockSize );
			void deallocate( void *p, std::size_t blockSize );

			unsigned char *data;
			unsigned char firstAvailableBlock;
			unsigned char blocksAvailable;
		};

	}

}

#endif

