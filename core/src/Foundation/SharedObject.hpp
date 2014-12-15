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

#ifndef CRIMILD_CORE_FOUNDATION_SHARED_OBJECT_
#define CRIMILD_CORE_FOUNDATION_SHARED_OBJECT_

#include "Macros.hpp"

#include <thread>
#include <memory>

namespace crimild {

    class SharedObject : public std::enable_shared_from_this< SharedObject > {
		CRIMILD_DISALLOW_COPY_AND_ASSIGN( SharedObject )
        
	protected:
        SharedObject( void ) { }

	public:
        virtual ~SharedObject( void ) { }
        
        std::shared_ptr< SharedObject > getShared( void )
        {
            return shared_from_this();
        }
        
        template< class T >
        std::shared_ptr< T > getShared( void )
        {
            return std::static_pointer_cast< T >( shared_from_this() );
        }
        
        void lock( void ) { _mutex.lock(); }
        
        void unlock( void ) { _mutex.unlock(); }

	private:
        std::mutex _mutex;
	};
    
    using SharedObjectPtr = std::shared_ptr< SharedObject >;
    
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

