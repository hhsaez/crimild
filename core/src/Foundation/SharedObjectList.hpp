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

#include "Mathematics/Random.hpp"

#include <list>
#include <functional>

namespace crimild {
    
    template< class T >
    class NoCallbacksPolicy {
    public:
        void onAdded( SharedPointer< T > const &object ) { }
        void onRemoved( SharedPointer< T > const &object ) { }
    };
    
    template< class T >
    class InvokeCallbacksPolicy {
        typedef std::function< void( SharedPointer< T > const & ) > CallbackType;
    public:
        void setOnAddCallback( CallbackType callback ) { _onAddedCallback = callback; }
        
        void onAdded( SharedPointer< T > const &object )
        {
            _onAddedCallback( object );
        }

        void setOnRemovedCallback( CallbackType callback ) { _onRemovedCallback = callback; }

        void onRemoved( SharedPointer< T > const &object )
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
        public SharedObject,
        public CallbackPolicy< ObjectType > {
    private:
        typedef SharedPointer< ObjectType > ObjectPtr;
        typedef CallbackPolicy< ObjectType > CallbackPolicyImpl;
        
    public:
        SharedObjectList( void )
        {
            
        }
        
        virtual ~SharedObjectList( void )
        {
            clear();
        }
            
        std::size_t size( void ) const { return _objects.size(); }
        
        bool isEmpty( void ) const { return _objects.size() == 0; }
        
        void add( ObjectPtr const &obj )
        {
            _objects.push_back( obj );
            CallbackPolicyImpl::onAdded( obj );
        }
            
        void add( ObjectType *obj )
        {
            add( crimild::retain( obj ) );
        }
            
        void addFront( ObjectPtr const &obj )
        {
            _objects.push_front( obj );
            CallbackPolicyImpl::onAdded( obj );
        }
        
        void addFront( ObjectType *obj )
        {
            addFront( crimild::retain( obj ) );
        }
        
        ObjectPtr remove( ObjectPtr const &obj )
        {
            _objects.remove( obj );
            CallbackPolicyImpl::onRemoved( obj );
            return obj;
        }
            
        ObjectPtr remove( ObjectType *obj )
        {
            return remove( std::move( crimild::retain( obj ) ) );
        }
            
        ObjectPtr popFront( void )
        {
            auto obj = _objects.front();
            _objects.pop_front();
            CallbackPolicyImpl::onRemoved( obj );
            return obj;
        }
            
        ObjectPtr popBack( void )
        {
            auto obj = _objects.back();
            _objects.pop_back();
            CallbackPolicyImpl::onRemoved( obj );
            return obj;
        }
    
        void clear( void )
        {
            auto os = _objects;
            for ( auto obj : os ) CallbackPolicyImpl::onRemoved( obj );
            
            _objects.clear();
        }
        
        void each( std::function< void( ObjectType *, int index ) > callback )
        {
            int i = 0;
            auto os = _objects;
            for ( auto o : os ) callback( crimild::get_ptr( o ), i++ );
        }
            
        ObjectType *find( std::function< bool( ObjectPtr const & ) > callback )
        {
            ObjectType result = nullptr;
            auto os = _objects;
            for ( auto o : os ) {
                if ( callback( o ) ) {
                    result = crimild::get_ptr( o );
                    break;
                }
            }
            
            return result;
        }
            
        ObjectType *front( void )
        {
            return _objects.front();
        }
            
        void shuffle( void )
        {
            Random::shuffle( _objects );
        }
        
    private:
        std::list< ObjectPtr > _objects;
    };
    
}

#endif

