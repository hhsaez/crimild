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

#ifndef CRIMILD_MEMORY_SMALL_OBJECT_
#define CRIMILD_MEMORY_SMALL_OBJECT_

#include "Common/Macros.hpp"
#include "Memory/SmallObjectAllocator.hpp"
#include "Policies/NonCopyable.hpp"

#include <mutex>
#include <thread>

namespace crimild {

    template< class Allocator = DefaultSmallObjectAllocator >
    class SmallObject : public NonCopyable {
    private:
        using Mutex = std::mutex;
        using Lock = std::lock_guard< Mutex >;

    public:
        static void *operator new( std::size_t size )
        {
            Lock lock( _mutex );
            return Allocator::getInstance()->allocate( size );
        }

        static void operator delete( void *p, std::size_t size )
        {
            Lock lock( _mutex );
            Allocator::getInstance()->deallocate( p, size );
        }

    private:
        static Mutex _mutex;

    protected:
        SmallObject( void )
        {
        }

    public:
        virtual ~SmallObject( void )
        {
        }
    };

    template< typename Allocator >
    typename SmallObject< Allocator >::Mutex SmallObject< Allocator >::_mutex;

}

#endif
