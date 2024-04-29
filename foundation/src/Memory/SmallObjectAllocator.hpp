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

#ifndef CRIMILD_MEMORY_SMALL_OBJECT_ALLOCATOR_
#define CRIMILD_MEMORY_SMALL_OBJECT_ALLOCATOR_

#include "Common/Singleton.hpp"
#include "Memory/FixedAllocator.hpp"

#include <iostream>

#ifndef CRIMILD_DEFAULT_CHUNK_SIZE
    #define CRIMILD_DEFAULT_CHUNK_SIZE 4096
#endif

#ifndef CRIMILD_MAX_SMALL_OBJECT_SIZE
    #define CRIMILD_MAX_SMALL_OBJECT_SIZE 2048 // sizeof( Group ) > 300
#endif

#ifndef CRIMILD_DEFAULT_OBJECT_ALIGNMENT
    #define CRIMILD_DEFAULT_OBJECT_ALIGNMENT 4
#endif

namespace crimild {

    class SmallObjectAllocator : public StaticSingleton< SmallObjectAllocator > {
    private:
        inline static std::size_t getOffset( std::size_t numBytes, std::size_t alignment );
        inline static void *defaultAlloc( std::size_t numBytes );
        inline static void defaultDealloc( void *p );

    public:
        SmallObjectAllocator( std::size_t pageSize = CRIMILD_DEFAULT_CHUNK_SIZE, std::size_t maxObjectSize = CRIMILD_MAX_SMALL_OBJECT_SIZE, std::size_t objectAlignSize = CRIMILD_DEFAULT_OBJECT_ALIGNMENT );
        ~SmallObjectAllocator( void );

        void *allocate( std::size_t numBytes );
        void deallocate( void *p, std::size_t size );

        const std::size_t getMaxObjectSize( void ) const { return _maxObjectSize; }
        const std::size_t getAlignment( void ) const { return _objectAlignSize; }

    private:
        bool trimExcessMemory( void );

    private:
        internal::FixedAllocator *_pool = nullptr;

        std::size_t _maxObjectSize;
        std::size_t _objectAlignSize;
    };

    using DefaultSmallObjectAllocator = SmallObjectAllocator;

}

#endif
