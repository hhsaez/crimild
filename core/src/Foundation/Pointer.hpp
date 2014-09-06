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

#ifndef CRIMILD_CORE_FOUNDATION_POINTER_
#define CRIMILD_CORE_FOUNDATION_POINTER_

#include "SharedObject.hpp"

namespace crimild {
    
    template< class T >
    class Pointer {
    public:
        Pointer( void ) : _data( nullptr ) { }
        explicit Pointer( T *data ) : _data( data ) { if ( _data != nullptr ) _data->retain(); }
        Pointer( const Pointer &ptr ) : _data( ptr._data ) { if ( _data != nullptr ) _data->retain(); }
        
        template< class U >
        Pointer( const Pointer< U > &ptr ) : _data( ptr.get() ) { if ( _data != nullptr ) _data->retain(); }
        
        ~Pointer( void ) { if ( _data != nullptr ) { _data->release(); _data = nullptr; } }
        
        T &operator*( void ) const { return &_data; }
        T *operator->( void ) const { return _data; }
        
        T *get( void ) const { return _data; }
        
        void set( T *data )
        {
            if ( data != _data ) {
                if ( _data != nullptr ) {
                    _data->release();
                }
                
                _data = data;
                
                if ( _data != nullptr ) {
                    _data->retain();
                }
            }
        }
        
        Pointer &operator=( T *data )
        {
            set( data );
            return *this;
        }
        
        Pointer &operator=( const Pointer &ptr )
        {
            set( ptr.get() );
            return *this;
        }
        
        template< class U >
        Pointer &operator=( const Pointer< U > &ptr )
        {
            set( ptr.get() );
            return *this;
        }
        
        bool operator==( T *data ) const { return _data == data; }
        bool operator==( const Pointer &ptr ) const { return _data == ptr._data; }
        bool operator!=( T *data ) const { return _data != data; }
        bool operator!=( const Pointer &ptr ) const { return _data != ptr._data; }
        
    private:
        T *_data = nullptr;
    };
    
}

#endif

