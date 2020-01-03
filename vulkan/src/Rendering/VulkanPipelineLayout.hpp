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

#ifndef CRIMILD_RENDERING_VULKAN_PIPELINE_LAYOUT_
#define CRIMILD_RENDERING_VULKAN_PIPELINE_LAYOUT_

#include "Foundation/VulkanObject.hpp"

namespace crimild {

    namespace vulkan {

        class DescriptorSetLayout;
        class RenderDevice;
        class PipelineLayoutManager;

        class PipelineLayout : public VulkanObject {
            CRIMILD_IMPLEMENT_RTTI( crimild::vulkan::PipelineLayout )

        public:
            struct Descriptor {
                RenderDevice *renderDevice;
                std::vector< DescriptorSetLayout * > setLayouts;
            };

        public:
            ~PipelineLayout( void );

            RenderDevice *renderDevice = nullptr;
            PipelineLayoutManager *manager = nullptr;
            VkPipelineLayout handler = VK_NULL_HANDLE;
        };

        class PipelineLayoutManager : public VulkanObjectManager< PipelineLayout > {
        public:
            explicit PipelineLayoutManager( RenderDevice *renderDevice = nullptr ) noexcept : m_renderDevice( renderDevice ) { }
            virtual ~PipelineLayoutManager( void ) noexcept = default;

            SharedPointer< PipelineLayout > create( PipelineLayout::Descriptor const &descriptor ) noexcept;
            void destroy( PipelineLayout *pipelineLayout ) noexcept;

        private:
            RenderDevice *m_renderDevice = nullptr;
        };

    }

}

#endif


