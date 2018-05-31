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

#ifndef CRIMILD_CORE_FOUNDATION_SHARED_OBJECT_MAP_
#define CRIMILD_CORE_FOUNDATION_SHARED_OBJECT_MAP_

#include "Foundation/SharedObject.hpp"

#include <map>
#include <string>
#include <functional>

namespace crimild {
	
    /**
	   \deprecated Use containers::Map< SharedPointer< ObjectType >> instead
	*/
    template< class ObjectType >
    class SharedObjectMap : public SharedObject {
    private:
        typedef SharedPointer< ObjectType > ObjectPtr;
        
    public:
        SharedObjectMap( void )
        {
            
        }
        
        virtual ~SharedObjectMap( void )
        {
            clear();
        }
        
        bool isEmpty( void ) const { return _objects.size() == 0; }
        
        void add( std::string key, ObjectPtr const &obj )
        {
            _objects[ key ] = obj;
        }
        
        ObjectPtr remove( std::string key )
        {
            auto obj = _objects[ key ];
            _objects[ key ] = nullptr;
            return obj;
        }

        ObjectType *get( std::string key )
        {
            return crimild::get_ptr( _objects[ key ] );
        }
        
        void clear( void )
        {
            _objects.clear();
        }
        
        void each( std::function< void( std::string, ObjectType * ) > callback )
        {
            auto os = _objects;
            for ( auto o : os ) {
                if ( o.second != nullptr ) {
                    callback( o.first, crimild::get_ptr( o.second ) );
                }
            }
        }
        
    private:
        std::map< std::string, ObjectPtr > _objects;
    };

}

#endif

