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

#ifndef CRIMILD_FOUNDATION_CONCURRENT_LIST_
#define CRIMILD_FOUNDATION_CONCURRENT_LIST_

#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace crimild {
    
    template< class T >
    class ConcurrentList {
    public:
    private:
        using List = std::list< T >;
        using Mutex = std::mutex;
        using ScopedLock = std::unique_lock< Mutex >;
        using Condition = std::condition_variable;
        
    public:
        ConcurrentList( void )
        {
            
        }
        
        ~ConcurrentList( void )
        {
            
        }
        
        bool empty( void ) const
        {
            ScopedLock lock( _mutex );
            return _list.empty();
        }
        
        std::size_t size( void ) const
        {
            ScopedLock lock( _mutex );
            return _list.size();
        }
        
        void add( T const &value )
        {
            ScopedLock lock( _mutex );
            _list.push_back( value );
        }
        
        void remove( T const &value )
        {
            ScopedLock lock( _mutex );
            _list.remove( value );
        }
        
        void each( std::function< void( T const & ) > callback )
        {
            ScopedLock lock( _mutex );
            auto elems = _list;
            lock.unlock();
            
            for ( auto &e : elems ) callback( e );
        }
        
    private:
        List _list;
        mutable Mutex _mutex;
    };
    
}

#endif

