/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_RENDERING_RENDER_RESOURCE_
#define CRIMILD_RENDERING_RENDER_RESOURCE_

#include "Crimild_Foundation.hpp"

namespace crimild {

    template< typename RenderResourceType >
    class [[deprecated]] RenderResourceManager {
    public:
        virtual ~RenderResourceManager( void ) noexcept
        {
            clear();
        }

        virtual crimild::Bool bind( RenderResourceType *resource ) noexcept
        {
            m_resources.insert( resource );
            return true;
        }

        virtual crimild::Bool unbind( RenderResourceType *resource ) noexcept
        {
            m_resources.remove( resource );
            return true;
        }

        virtual void clear( void ) noexcept
        {
            m_resources.each(
                [ this ]( RenderResourceType *resource ) {
                    unbind( resource );
                }
            );
            m_resources.clear();
        }

        inline RenderResourceType *first( void ) noexcept
        {
            return m_resources.first();
        }

    private:
        Set< RenderResourceType * > m_resources;
    };

    // TODO: deprecate this in favor of keeping track of renderables in managers and
    // using messaging for bind/update/unbind
    template< typename T >
    class [[deprecated]] RenderResourceImpl {
    public:
        virtual ~RenderResourceImpl( void ) noexcept
        {
            if ( manager != nullptr ) {
                manager->unbind( static_cast< T * >( this ) );
            }
        }

        RenderResourceManager< T > *manager = nullptr;
    };

    template< typename RenderResourceType >
    class [[deprecated]] RenderResourceLibrary : public StaticSingleton< RenderResourceLibrary< RenderResourceType > > {
    private:
        using Builder = std::function< SharedPointer< RenderResourceType >( void ) >;

    public:
        RenderResourceLibrary( void ) noexcept { }
        virtual ~RenderResourceLibrary( void ) = default;

        void add( std::string name, SharedPointer< RenderResourceType > const &shader )
        {
            m_resources[ name ] = shader;
        }

        void add( std::string name, Builder const &builder )
        {
            m_builders[ name ] = builder;
        }

        RenderResourceType *get( std::string name )
        {
            if ( !m_resources.contains( name ) && m_builders.contains( name ) ) {
                // Lazy create and add a new resource instance
                // if the resource does not exist but there's a builder registered
                add( name, m_builders[ name ]() );
            }

            // Check again, because we may have just added the resource
            if ( !m_resources.contains( name ) ) {
                return nullptr;
            }

            return crimild::get_ptr( m_resources[ name ] );
        }

    private:
        Map< std::string, SharedPointer< RenderResourceType > > m_resources;
        Map< std::string, Builder > m_builders;
    };

}

#endif
