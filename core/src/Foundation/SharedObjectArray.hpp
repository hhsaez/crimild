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

#ifndef CRIMILD_CORE_FOUNDATION_SHARED_OBJECT_ARRAY_
#define CRIMILD_CORE_FOUNDATION_SHARED_OBJECT_ARRAY_

#include "Foundation/SharedObject.hpp"

#include <vector>
#include <functional>

namespace crimild {
    
    template< class ObjectType >
    class SharedObjectArray {
    private:
        typedef SharedPointer< ObjectType > ObjectPtr;
        
    public:
        SharedObjectArray( void )
        {
            
        }

        SharedObjectArray( const SharedObjectArray &other )
        {
            _objects = other._objects;
            _objectCount = other._objectCount;
        }
        
        virtual ~SharedObjectArray( void )
        {
            clear();
        }

        SharedObjectArray &operator=( const SharedObjectArray &other )
        {
            _objects = other._objects;
            _objectCount = other._objectCount;
            return *this;
        }

        bool empty( void ) const { return _objectCount == 0; }

        int size( void ) const { return _objectCount; }
        
        void add( ObjectType *obj )
        {
            add( crimild::retain( obj ) );
        }
        
        void add( ObjectPtr const &obj )
        {
            bool added = false;
            for ( int i = 0; i < _objects.size(); i++ ) {
                if ( _objects[ i ] == nullptr ) {
                    _objects[ i ] = obj;
                    added = true;
                    break;
                }
            }

            if ( !added ) _objects.push_back( obj );
            ++_objectCount;
        }
        
        ObjectPtr remove( ObjectType *obj )
        {
            for ( int i = 0; i < _objects.size(); i++ ) {
                if ( get_ptr( _objects[ i ] ) == obj ) {
                    auto result = _objects[ i ];
                    _objects[ i ] = nullptr;
                    --_objectCount;
                    return result;
                }
            }
            
            return nullptr;
        }
        
        ObjectPtr remove( ObjectPtr const &obj )
        {
            return remove( get_ptr( obj ) );
        }
        
        void clear( void )
        {
            _objects.clear();
            _objectCount = 0;
        }

        ObjectType *get( int index )
        {
            if ( index >= _objects.size() ) {
                return nullptr;
            }

            return get_ptr( _objects[ index ] );
        }
        
        void forEach( std::function< void( ObjectType *, int index ) > callback, bool ignoreNulls = true )
        {
            if ( empty() ) {
                return;
            }

            int i = 0;
            for ( auto &o : _objects ) {
                if ( !ignoreNulls || o != nullptr ) {
                    callback( crimild::get_ptr( o ), i++ );
                }
            }
        }
            
        void forEach( std::function< void( ObjectType * ) > callback, bool ignoreNulls = true )
        {
            forEach( [callback]( ObjectType *obj, int index ) { callback( obj ); }, ignoreNulls );
        }
            
    private:
        std::vector< ObjectPtr > _objects;
        int _objectCount = 0;
    };
    
}

#endif

