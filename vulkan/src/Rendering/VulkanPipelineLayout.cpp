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

#include "Rendering/VulkanPipelineLayout.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;
using namespace crimild::vulkan;

PipelineLayout::~PipelineLayout( void )
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

SharedPointer< PipelineLayout > PipelineLayoutManager::create( PipelineLayout::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan pipeline layout" );

    auto renderDevice = m_renderDevice;
    if ( renderDevice == nullptr ) {
        renderDevice = descriptor.renderDevice;
    }

    auto createInfo = VkPipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    VkPipelineLayout pipelineLayoutHandler;
    if ( vkCreatePipelineLayout( renderDevice->handler, &createInfo, nullptr, &pipelineLayoutHandler ) != VK_SUCCESS ) {
        CRIMILD_LOG_ERROR( "Failed to create Vulkan pipeline layout" );
        return nullptr;
    }

    auto pipelineLayout = crimild::alloc< PipelineLayout >();
    pipelineLayout->manager = this;
    pipelineLayout->renderDevice = renderDevice;
    pipelineLayout->handler = pipelineLayoutHandler;
    insert( crimild::get_ptr( pipelineLayout ) );

    return pipelineLayout;
}

void PipelineLayoutManager::destroy( PipelineLayout *pipelineLayout ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying Vulkan pipeline layout" );

    if ( pipelineLayout->renderDevice != nullptr
         && pipelineLayout->handler != VK_NULL_HANDLE ) {
        vkDestroyPipelineLayout(
        	pipelineLayout->renderDevice->handler,
            pipelineLayout->handler,
        	nullptr
        );
    }

    pipelineLayout->handler = VK_NULL_HANDLE;
    pipelineLayout->manager = nullptr;
    pipelineLayout->renderDevice = nullptr;
    erase( pipelineLayout );
}
