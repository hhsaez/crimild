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

crimild::Bool DescriptorSetManager::bind( DescriptorSet *descriptorSet ) noexcept
{
    if ( validate( descriptorSet ) ) {
        return true;
    }

    CRIMILD_LOG_TRACE( "Binding Vulkan descriptor set" );

    auto renderDevice = getRenderDevice();
    if ( renderDevice == nullptr ) {
        return false;
    }

    auto swapchain = renderDevice->getSwapchain();
    auto descriptorPool = crimild::get_ptr( descriptorSet->descriptorPool );
    auto layout = crimild::get_ptr( descriptorSet->descriptorSetLayout );
    auto count = swapchain->images.size();

    VkDescriptorSetLayout layouts[] = {
        renderDevice->getHandler( layout ),
    };

    auto allocInfo = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = renderDevice->getHandler( descriptorPool ),
        .descriptorSetCount = 1,
        .pSetLayouts = layouts,
    };

    containers::Array< VkDescriptorSet > handlers( count );

    for ( int i = 0; i < count; i++ ) {
        VkDescriptorSet handler;
        CRIMILD_VULKAN_CHECK(
            vkAllocateDescriptorSets(
                renderDevice->handler,
                &allocInfo,
                &handler
             )
        );
        handlers[ i ] = handler;
    }

    setHandlers( descriptorSet, handlers );

    // Keep these alive until all operations are completed
    VkDescriptorBufferInfo bufferInfo;
    VkDescriptorImageInfo imageInfo;

    for ( auto i = 0l; i < count; ++i ) {
        std::vector< VkWriteDescriptorSet > writes( descriptorSet->writes.size() );
        for ( auto j = 0l; j < writes.size(); ++j ) {
            auto &write = descriptorSet->writes[ j ];
            writes[ j ] = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = handlers[ i ],
                .dstBinding = static_cast< crimild::UInt32 >( j ),
                .dstArrayElement = 0,
                .descriptorType = utils::getVulkanDescriptorType( write.descriptorType ),
                .descriptorCount = 1,
            };

            if ( write.descriptorType == DescriptorType::UNIFORM_BUFFER ) {
                auto buffer = write.buffer;
                auto bindInfo = renderDevice->getBindInfo( buffer );
                auto bufferHandler = bindInfo.bufferHandlers[ i ];
                bufferInfo.buffer = bufferHandler;
                bufferInfo.offset = 0;
                bufferInfo.range = write.buffer->getSize();
                writes[ j ].pBufferInfo = &bufferInfo;
            }
            else if ( write.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER ) {
//                auto texture = write.texture;
//                auto bindInfo = renderDevice->getBindInfo( texture );
//                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//                imageInfo.imageView = bindInfo.imageView->handler;
//                imageInfo.sampler = bindInfo.sampler;
//                writes[ j ].pImageInfo = &imageInfo;
            }

            vkUpdateDescriptorSets(
                renderDevice->handler,
                1,
                &writes[ j ],
                0,
                nullptr
            );
        }
    }

    return ManagerImpl::bind( descriptorSet );

}

crimild::Bool DescriptorSetManager::unbind( DescriptorSet *descriptorSet ) noexcept
{
    if ( !validate( descriptorSet ) ) {
        return false;
    }

    CRIMILD_LOG_TRACE( "Unbinding Vulkan descriptor set" );

    // No need to explicitly delete descriptor sets

    removeHandlers( descriptorSet );

    return ManagerImpl::unbind( descriptorSet );
}

