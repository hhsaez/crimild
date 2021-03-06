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

	/**
	   \deprecated Use containers::List< T, policies::ObjectLevelLockable > instead
	 */
    template< class T >
    class ConcurrentList {
    public:
    private:
        using List = std::list< T >;
        using Mutex = std::mutex;
        using ScopedLock = std::lock_guard< Mutex >;
        
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
        
        unsigned int size( void ) const
        {
            ScopedLock lock( _mutex );
            return _list.size();
        }
        
        void push_back( T const &value )
        {
            ScopedLock lock( _mutex );
            _list.push_back( value );
        }
        
        void push_front( T const &value )
        {
            ScopedLock lock( _mutex );
            _list.push_front( value );
        }
        
        void remove( T const &value )
        {
            ScopedLock lock( _mutex );
            _list.remove( value );
        }
        
        void clear( void )
        {
            ScopedLock lock( _mutex );
            _list.clear();
        }
        
        /**
            \brief Traverses the list 
         
            This method will create a temporary copy of the list. That way, the callbacks
            may add new elements if needed at the same time
         
            \param flush If true, the list will be cleared before iterating the elems
         */
        void each( std::function< void( T const & ) > callback, bool flush = false )
        {
            List elems;

            {
                ScopedLock lock( _mutex );
                elems = _list;
                
                if ( flush ) {
                    _list.clear();
                }
            }
            
            for ( auto &e : elems ) callback( e );
        }
        
    private:
        List _list;
        mutable Mutex _mutex;
    };
    
}

#endif

