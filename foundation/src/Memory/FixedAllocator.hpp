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

#ifndef CRIMILD_MEMORY_FIXED_ALLOCATOR_
#define CRIMILD_MEMORY_FIXED_ALLOCATOR_

#include "Common/Macros.hpp"
#include "Memory/Chunk.hpp"
#include "Policies/NonCopyable.hpp"

#include <limits>
#include <vector>

namespace crimild {

    namespace internal {

        class FixedAllocator : public NonCopyable {
        private:
            static constexpr unsigned char MIN_OBJECTS_PER_CHUNK = 8;
            static constexpr unsigned char MAX_OBJECTS_PER_CHUNK = std::numeric_limits< unsigned char >::max();

        private:
            using ChunkArray = std::vector< Chunk >;
            using ChunkIterator = ChunkArray::iterator;
            using ChunkConstIterator = ChunkArray::const_iterator;

        public:
            FixedAllocator( void );
            ~FixedAllocator( void );

            void init( std::size_t blockSize, std::size_t pageSize );

            void *allocate( void );
            bool deallocate( void *p, Chunk *hint );

            std::size_t countEmptyChunks( void ) const;

            inline std::size_t getBlockSize( void ) const { return _blockSize; }

            bool trimEmptyChunk( void );
            bool trimChunkList( void );

            Chunk *hasBlock( void *p );

        private:
            bool makeNewChunk( void );

            Chunk *vicinityFind( void *p ) const;
            void doDeallocate( void *p );

        private:
            std::size_t _blockSize;
            unsigned char _numBlocks;

            ChunkArray _chunks;
            Chunk *_allocChunk = nullptr;
            Chunk *_deallocChunk = nullptr;
            Chunk *_emptyChunk = nullptr;
        };

    }

}

#endif
