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
*     * Neither the name of the copyright holders nor the
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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_RESOURCE_
#define CRIMILD_VULKAN_RENDERING_RENDER_RESOURCE_

#include "Foundation/VulkanUtils.hpp"
#include "Rendering/RenderResource.hpp"
#include "Foundation/Containers/Array.hpp"
#include "Foundation/Containers/Map.hpp"

namespace crimild {

    namespace vulkan {

        class RenderDevice;

        template< typename T >
        class VulkanRenderResourceManager : public RenderResourceManager< T > {
        protected:
            VulkanRenderResourceManager( RenderDevice *renderDevice = nullptr ) noexcept : m_renderDevice( renderDevice ) { }
        public:
            virtual ~VulkanRenderResourceManager( void ) noexcept = default;

            RenderDevice *getRenderDevice( void ) noexcept { return m_renderDevice; }

        private:
            RenderDevice *m_renderDevice = nullptr;
        };

        template<
        	typename RESOURCE_TYPE,
        	typename HANDLER_TYPE
        >
        class VulkanRenderResourceManagerImpl : public RenderResourceManager< RESOURCE_TYPE > {
        protected:
            VulkanRenderResourceManagerImpl( RenderDevice *renderDevice = nullptr ) noexcept
            	: m_renderDevice( renderDevice )
            {
                if ( m_renderDevice == nullptr ) {
                    // If render device is null, assume that we're the render device
                    m_renderDevice = static_cast< RenderDevice * >( this );
                }
            }

        public:
            virtual ~VulkanRenderResourceManagerImpl( void ) noexcept = default;

            RenderDevice *getRenderDevice( void ) noexcept { return m_renderDevice; }

        private:
            RenderDevice *m_renderDevice = nullptr;
        };

        template< typename ResourceType, typename HandlerType >
        class SingleHandlerRenderResourceManagerImpl : public VulkanRenderResourceManagerImpl< ResourceType, HandlerType > {
        public:
            virtual ~SingleHandlerRenderResourceManagerImpl( void ) noexcept = default;

            crimild::Bool validate( ResourceType *resource ) const noexcept
            {
                return m_handlers.contains( resource );
            }

            HandlerType getHandler( ResourceType *resource ) noexcept
            {
                if ( !validate( resource ) && !this->bind( resource ) ) {
                    return VK_NULL_HANDLE;
                }
                return m_handlers[ resource ];
            }

        protected:
            void setHandler( ResourceType *resource, HandlerType handler ) noexcept
            {
                m_handlers[ resource ] = handler;
            }

            void removeHandlers( ResourceType *resource ) noexcept
            {
                if ( !validate( resource ) ) {
                    return;
                }
                m_handlers.remove( resource );
            }

        private:
            containers::Map< ResourceType *, HandlerType > m_handlers;
        };

        template< typename ResourceType, typename HandlerType >
        class MultiHandlerRenderResourceManagerImpl : public VulkanRenderResourceManagerImpl< ResourceType, HandlerType > {
            using HandlerArray = containers::Array< HandlerType >;

        public:
            virtual ~MultiHandlerRenderResourceManagerImpl( void ) noexcept = default;

            crimild::Bool validate( ResourceType *resource ) const noexcept
            {
                return m_handlers.contains( resource ) && m_handlers[ resource ].size() > 0;
            }

            HandlerType getHandler( ResourceType *resource, crimild::Size index ) noexcept
            {
                if ( !validate( resource ) && !this->bind( resource ) ) {
                    return VK_NULL_HANDLE;
                }
                auto &handlers = m_handlers[ resource ];
                if ( index < handlers.size() ) {
                    index = handlers.size() - 1;
                }
                return m_handlers[ resource ][ index ];
            }

        protected:
            void setHandlers( ResourceType *resource, const HandlerArray &handlers ) noexcept
            {
                m_handlers[ resource ] = handlers;
            }

            template< typename Fn >
            void eachHandler( ResourceType *resource, Fn const &fn )
            {
                m_handlers[ resource ].each( fn );
            }

            void removeHandlers( ResourceType *resource ) noexcept
            {
                if ( !validate( resource ) ) {
                    return;
                }
                m_handlers[ resource ].clear();
                m_handlers.remove( resource );
            }

        private:
            containers::Map< ResourceType *, HandlerArray > m_handlers;
        };

    }

}

#endif

