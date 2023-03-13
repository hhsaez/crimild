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

#include "Rendering/VulkanDescriptorSet.hpp"

#include "Rendering/BufferView.hpp"
#include "Rendering/VulkanBuffer.hpp"
#include "Rendering/VulkanDescriptorPool.hpp"
#include "Rendering/VulkanDescriptorSetLayout.hpp"
#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanSampler.hpp"

#include <array>

using namespace crimild::vulkan;

DescriptorSet::DescriptorSet(
    RenderDevice *device,
    std::string name,
    std::shared_ptr< DescriptorPool > pool,
    std::shared_ptr< DescriptorSetLayout > layout,
    const std::vector< Descriptor > &descriptors
) noexcept
    : Named( name ),
      WithRenderDevice( device ),
      m_pool( pool ),
      m_layout( layout ),
      m_descriptors( descriptors )
{
    std::array< VkDescriptorSetLayout, 1 > layouts = { layout->getHandle() };
    const auto info = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = pool->getHandle(),
        .descriptorSetCount = 1,
        .pSetLayouts = layouts.data(),
    };

    VkDescriptorSet handle = VK_NULL_HANDLE;
    CRIMILD_VULKAN_CHECK(
        vkAllocateDescriptorSets(
            getRenderDevice()->getHandle(),
            &info,
            &handle
        )
    );

    if ( !name.empty() ) {
        device->setObjectName( handle, name );
    }

    setHandle( handle );

    std::vector< VkWriteDescriptorSet > writes;
    std::vector< VkDescriptorImageInfo > imageInfos;
    std::vector< VkDescriptorBufferInfo > bufferInfos;
    for ( auto &descriptor : m_descriptors ) {
        switch ( descriptor.type ) {
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
                bufferInfos.push_back(
                    VkDescriptorBufferInfo {
                        .buffer = descriptor.buffer->getHandle(),
                        .offset = 0,
                        .range = descriptor.buffer->getBufferView()->getLength(),
                    }
                );
                writes.push_back(
                    VkWriteDescriptorSet {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = getHandle(),
                        .dstBinding = uint32_t( writes.size() ),
                        .dstArrayElement = 0,
                        .descriptorCount = 1,
                        .descriptorType = descriptor.type,
                        .pImageInfo = nullptr,
                        .pBufferInfo = &bufferInfos.back(),
                        .pTexelBufferView = nullptr,
                    }
                );
                break;
            }

            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: {
                imageInfos.emplace_back(
                    VkDescriptorImageInfo {
                        .sampler = descriptor.sampler->getHandle(),
                        .imageView = descriptor.imageView->getHandle(),
                        .imageLayout =
                            getRenderDevice()->formatIsColor( descriptor.imageView->getImage()->getFormat() )
                                ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                                : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
                    }
                );
                writes.emplace_back(
                    VkWriteDescriptorSet {
                        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                        .dstSet = getHandle(),
                        .dstBinding = uint32_t( writes.size() ),
                        .dstArrayElement = 0,
                        .descriptorCount = 1,
                        .descriptorType = descriptor.type,
                        .pImageInfo = &imageInfos.back(),
                        .pBufferInfo = nullptr,
                        .pTexelBufferView = nullptr,
                    }
                );
                break;
            }

            default: {
                CRIMILD_LOG_FATAL( "Unrecognized descriptor type: ", descriptor.type );
                exit( -1 );
            }
        }
    }

    vkUpdateDescriptorSets(
        getRenderDevice()->getHandle(),
        writes.size(),
        writes.data(),
        0,
        nullptr
    );
}

DescriptorSet::DescriptorSet(
    RenderDevice *device,
    std::string name,
    const std::vector< Descriptor > &descriptors
) noexcept
    : DescriptorSet(
        device,
        name,
        crimild::alloc< DescriptorPool >( device, name + "/DescriptorPool", descriptors ),
        crimild::alloc< DescriptorSetLayout >( device, name + "/DescriptorSetLayout", descriptors ),
        descriptors
    )
{
    // no-op
}

DescriptorSet::~DescriptorSet( void ) noexcept
{
    // No need to destroy descriptor sets, since they'll go away when the pool gets destroyed.
    setHandle( VK_NULL_HANDLE );

    m_descriptors.clear();

    m_pool = nullptr;
    m_layout = nullptr;
}

void DescriptorSet::updateDescriptors( void ) noexcept
{
    for ( auto &descriptor : m_descriptors ) {
        if ( descriptor.buffer != nullptr ) {
            descriptor.buffer->update();
        }
    }
}