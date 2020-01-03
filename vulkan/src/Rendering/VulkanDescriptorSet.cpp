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

#include "Rendering/VulkanDescriptorSet.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;
using namespace crimild::vulkan;

DescriptorSet::~DescriptorSet( void ) noexcept
{
    if ( manager != nullptr ) {
        manager->destroy( this );
    }
}

void DescriptorSet::write( Buffer *buffer, crimild::Size offset, crimild::Size size ) noexcept
{
    auto bufferInfo = VkDescriptorBufferInfo {
		.buffer = buffer->handler,
        .offset = offset,
        .range = size,
    };

    auto descriptorWrite = VkWriteDescriptorSet {
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = handler,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // TODO: type can change based on uniform usage?
        .descriptorCount = 1,
        .pBufferInfo = &bufferInfo,
        .pImageInfo = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(
        renderDevice->handler,
        1,
        &descriptorWrite,
        0,
        nullptr
   	);
}

SharedPointer< DescriptorSet > DescriptorSetManager::create( DescriptorSet::Descriptor const &descriptor ) noexcept
{
    CRIMILD_LOG_TRACE( "Creating Vulkan descriptor set" );

    auto renderDevice = m_renderDevice;
    if ( renderDevice == nullptr ) {
        renderDevice = descriptor.renderDevice;
    }

    auto descriptorPool = descriptor.descriptorPool;
    auto layout = descriptor.layout;

    VkDescriptorSetLayout layouts[] = {
        layout->handler
    };

    auto allocInfo = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool->handler,
        .descriptorSetCount = 1,
        .pSetLayouts = layouts,
    };

    VkDescriptorSet handler;
    CRIMILD_VULKAN_CHECK(
        vkAllocateDescriptorSets(
        	renderDevice->handler,
        	&allocInfo,
            &handler
     	)
    );

    auto descriptorSet = crimild::alloc< DescriptorSet >();
    descriptorSet->renderDevice = renderDevice;
    descriptorSet->manager = this;
    descriptorSet->handler = handler;
    insert( crimild::get_ptr( descriptorSet ) );
    return descriptorSet;
}

void DescriptorSetManager::destroy( DescriptorSet *descriptorSet ) noexcept
{
    CRIMILD_LOG_TRACE( "Destroying Vulkan descriptor set" );

    // No need to explicitly delete descriptor sets

    descriptorSet->manager = nullptr;
    descriptorSet->renderDevice = nullptr;
    descriptorSet->handler = VK_NULL_HANDLE;
    erase( descriptorSet );
}

