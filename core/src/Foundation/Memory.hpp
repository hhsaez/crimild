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
    using WeakPointer = std::weak_ptr< T >;

    template< typename T, typename... Args >
    SharedPointer< T > alloc( Args &&... args )
    {
        return std::make_shared< T >( std::forward< Args >( args )... );
    }

    template< typename T >
    using UniquePointer = std::unique_ptr< T >;

    template< typename T, typename... Args >
    UniquePointer< T > allocUnique( Args &&... args )
    {
        return std::unique_ptr< T >( new T( std::forward< Args >( args )... ) );        
    }

    template< typename T >
    bool pointerIsValid( WeakPointer< T > const &ptr )
    {
        return !ptr.expired();
    }

    template< typename T >
    WeakPointer< T > getWeakPointer( SharedPointer< T > const &ptr )
    {
        return ptr;
    }

    template< typename T >
    SharedPointer< T > getSharedPointer( WeakPointer< T > const &ptr )
    {
        return ptr.lock();
    }

    template< typename T >
    void resetPointer( SharedPointer< T > &ptr )
    {
        ptr = nullptr;
    }

    template< typename T >
    void resetPointer( WeakPointer< T > &ptr )
    {
        ptr.reset();
    }
    
}

namespace std {
    
    // from http://stackoverflow.com/a/6066150
    template< class T, class U >
    std::weak_ptr< T > static_pointer_cast( std::weak_ptr< U > const &r )
    {
        return std::static_pointer_cast< T >( std::shared_ptr< U >( r ) );
    }
}

#endif

