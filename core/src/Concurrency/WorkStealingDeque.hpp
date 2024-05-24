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

#ifndef CRIMILD_CORE_CONCURRENCY_WORK_STEALING_QUEUE_
#define CRIMILD_CORE_CONCURRENCY_WORK_STEALING_QUEUE_

#include "Crimild_Foundation.hpp"

#include <list>
#include <mutex>
#include <vector>

namespace crimild {

    /**
       \brief A double-ended queue implemeting the work stealing pattern
     */
    template< class T >
    class WorkStealingQueue : public SharedObject {
        using Mutex = std::mutex;
        using Lock = std::lock_guard< Mutex >;

    public:
        WorkStealingQueue( void )
        {
        }

        ~WorkStealingQueue( void )
        {
        }

        size_t size( void )
        {
            Lock lock( _mutex );
            return _elems.size();
        }

        bool empty( void )
        {
            return size() == 0;
        }

        void clear( void )
        {
            Lock lock( _mutex );

            _elems.clear();
        }

        /**
           \brief Adds an element to the private end of the queue (LIFO)

         */
        void push( T const &elem )
        {
            Lock lock( _mutex );
            _elems.push_back( elem );
        }

        /**
           \brief Retrieves an element from the private end of the queue (LIFO)

           \warning You should check if the collection is empty before calling this method.
         */
        T pop( void )
        {
            Lock lock( _mutex );

            if ( _elems.size() == 0 ) {
                return nullptr;
            }

            auto e = _elems.back();
            _elems.pop_back();
            return e;
        }

        /**
           \brief Retrieves an element from the public end of the queue (FIFO)

           \warning You should check if the collection is empty before calling this method
         */
        T steal( void )
        {
            Lock lock( _mutex );

            if ( _elems.size() == 0 ) {
                return nullptr;
            }

            auto e = _elems.front();
            _elems.pop_front();
            return e;
        }

    private:
        std::list< T > _elems;
        std::mutex _mutex;
    };

}

#endif
