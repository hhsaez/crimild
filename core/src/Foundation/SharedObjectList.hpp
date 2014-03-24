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

#ifndef CRIMILD_CORE_FOUNDATION_SHARED_OBJECT_LIST_
#define CRIMILD_CORE_FOUNDATION_SHARED_OBJECT_LIST_

#include "Foundation/SharedObject.hpp"
#include "Foundation/Pointer.hpp"

#include <list>
#include <functional>

namespace crimild {
    
    template< class T >
    class NoCallbacksPolicy {
    public:
        void onAdded( T *object ) { }
        void onRemoved( T *object ) { }
    };
    
    template< class T >
    class InvokeCallbacksPolicy {
        typedef std::function< void( T * ) > CallbackType;
    public:
        void setOnAddCallback( CallbackType callback ) { _onAddedCallback = callback; }
        
        void onAdded( T *object )
        {
            _onAddedCallback( object );
        }

        void setOnRemovedCallback( CallbackType callback ) { _onRemovedCallback = callback; }

        void onRemoved( T *object )
        {
            _onRemovedCallback( object );
        }
        
    private:
        CallbackType _onAddedCallback;
        CallbackType _onRemovedCallback;
    };
    
    template<
        class ObjectType,
        template < class > class CallbackPolicy = NoCallbacksPolicy
    >
    class SharedObjectList :
        SharedObject,
        CallbackPolicy< ObjectType > {
    private:
        typedef Pointer< ObjectType > ObjectPtr;
        typedef CallbackPolicy< ObjectType > CallbackPolicyImpl;
        
    public:
        SharedObjectList( void )
        {
            
        }
        
        virtual ~SharedObjectList( void )
        {
            clear();
        }
        
        bool isEmpty( void ) const { return _objects.size() == 0; }
        
        void add( ObjectType *obj )
        {
            ObjectPtr oPtr( obj );
            _objects.push_back( oPtr );
            CallbackPolicyImpl::onAdded( obj );
        }
        
        ObjectPtr remove( ObjectType *obj )
        {
            ObjectPtr oPtr( obj );
            _objects.remove( oPtr );
            CallbackPolicyImpl::onRemoved( obj );
            return oPtr;
        }
        
        void clear( void )
        {
            for ( auto oPtr : _objects ) {
                CallbackPolicyImpl::onRemoved( oPtr.get() );
            }
            
            _objects.clear();
        }
        
        void each( std::function< void( ObjectType *, int index ) > callback )
        {
            int i = 0;
            for ( auto oPtr : _objects ) {
                callback( oPtr.get(), i++ );
            }
        }
            
        ObjectType *find( std::function< bool( ObjectType * ) > callback )
        {
            ObjectType *result = nullptr;
            for ( auto oPtr : _objects ) {
                if ( callback( oPtr.get() ) ) {
                    result = oPtr.get();
                    break;
                }
            }
            
            return result;
        }
        
    private:
        std::list< ObjectPtr > _objects;
    };
    
}

#endif

