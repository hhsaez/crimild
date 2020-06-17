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

#ifndef CRIMILD_CORE_FOUNDATION_MEMORY_
#define CRIMILD_CORE_FOUNDATION_MEMORY_

#include <mutex>
#include <memory>

namespace crimild {

    template< typename T >
    using SharedPointer = std::shared_ptr< T >;

    template< typename T >
    using UniquePointer = std::unique_ptr< T >;
    
    template< typename T, typename... Args >
    SharedPointer< T > alloc( Args &&... args )
    {
        // use 'new' instead of 'make_shared' to force the use of
        // the custom allocator.
        // TODO: use alloc_shared?
        return SharedPointer< T >( new T( std::forward< Args >( args )... ) );
    }

    template< typename T, typename... Args >
    UniquePointer< T > alloc_unique( Args &&... args )
    {
        return std::unique_ptr< T >( new T( std::forward< Args >( args )... ) );        
    }
    
    template< typename T >
    SharedPointer< T > retain( T *ptr )
    {
        if ( ptr == nullptr ) {
            return nullptr;
        }
        
        return std::static_pointer_cast< T >( ptr->shared_from_this() );
    }
    
    template< typename T >
    inline SharedPointer< T > &retain( SharedPointer< T > &ptr ) noexcept
    {
        // Returns the same pointer.
        // This is a helper function that comes in handy for templates and generic code
        return ptr;
    }

    template< typename T >
    T *get_ptr( SharedPointer< T > const &ptr )
    {
        return ptr.get();
    }

    template< typename T >
    inline T *get_ptr( T *ptr ) noexcept
    {
        // Returns the same pointer.
        // This is a helper function that comes in handy for templates and generic code
        return ptr;
    }
    
    template< class T, class U >
    SharedPointer< T > cast_ptr( SharedPointer< U > const &ptr )
    {
        return std::static_pointer_cast< T >( ptr );
    }
    
    template< class T, class U >
    SharedPointer< T > dynamic_cast_ptr( SharedPointer< U > const &ptr )
    {
        return std::dynamic_pointer_cast< T >( ptr );
    }
    
}

#endif

