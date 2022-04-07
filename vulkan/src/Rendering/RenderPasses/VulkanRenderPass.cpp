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

#include "Rendering/RenderPasses/VulkanRenderPass.hpp"

#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;
using namespace crimild::vulkan;

vulkan::RenderPass::RenderPass( RenderDevice *renderDevice ) noexcept
    : m_renderDevice( renderDevice )
{
    // no-op
}

void vulkan::RenderPass::createFramebufferAttachment( std::string name, const VkExtent2D &extent, VkFormat format, FramebufferAttachment &out ) const
{
    CRIMILD_LOG_TRACE();

    const auto swapchainImageCount = getRenderDevice()->getSwapchainImageCount();

    if ( !getRenderDevice()->formatIsColor( format ) && !getRenderDevice()->formatIsDepthStencil( format ) ) {
        CRIMILD_LOG_ERROR( "Invalid attachment format ", format );
        return;
    }

    // Basic properties
    out.name = name;
    out.extent = extent;
    out.format = format;

    // Image
    getRenderDevice()->createImage(
        extent.width,
        extent.height,
        format,
        VK_IMAGE_TILING_OPTIMAL,
        getRenderDevice()->formatIsColor( format )
            ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
            : VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
        1,
        VK_SAMPLE_COUNT_1_BIT,
        1,
        0,
        out.image,
        out.memory );

    // Image View
    getRenderDevice()->createImageView(
        out.image,
        format,
        getRenderDevice()->formatIsColor( format )
            ? VK_IMAGE_ASPECT_COLOR_BIT
            : VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
        out.imageView );

    // Sampler
    auto samplerInfo = VkSamplerCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .mipLodBias = 0,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = 1,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .minLod = 0,
        .maxLod = 1,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
        .unnormalizedCoordinates = VK_FALSE,
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateSampler(
            getRenderDevice()->getHandle(),
            &samplerInfo,
            nullptr,
            &out.sampler ) );

    // Descriptor Set Layout
    const auto bindings = std::array< VkDescriptorSetLayoutBinding, 1 > {
        VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    auto layoutCreateInfo = VkDescriptorSetLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = uint32_t( bindings.size() ),
        .pBindings = bindings.data(),
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateDescriptorSetLayout(
            getRenderDevice()->getHandle(),
            &layoutCreateInfo,
            nullptr,
            &out.descriptorSetLayout ) );

    // Descriptor Pool
    const auto poolSizes = std::array< VkDescriptorPoolSize, 1 > {
        VkDescriptorPoolSize {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = uint32_t( swapchainImageCount ),
        },
    };

    auto poolCreateInfo = VkDescriptorPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = uint32_t( poolSizes.size() ),
        .pPoolSizes = poolSizes.data(),
        .maxSets = uint32_t( swapchainImageCount ),
    };

    CRIMILD_VULKAN_CHECK(
        vkCreateDescriptorPool(
            getRenderDevice()->getHandle(),
            &poolCreateInfo,
            nullptr,
            &out.descriptorPool ) );

    out.descriptorSets.resize( swapchainImageCount );

    std::vector< VkDescriptorSetLayout > layouts( swapchainImageCount, out.descriptorSetLayout );

    const auto allocInfo = VkDescriptorSetAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = out.descriptorPool,
        .descriptorSetCount = uint32_t( layouts.size() ),
        .pSetLayouts = layouts.data(),
    };

    CRIMILD_VULKAN_CHECK(
        vkAllocateDescriptorSets(
            getRenderDevice()->getHandle(),
            &allocInfo,
            out.descriptorSets.data() ) );

    for ( size_t i = 0; i < out.descriptorSets.size(); ++i ) {
        const auto imageInfo = VkDescriptorImageInfo {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = out.imageView,
            .sampler = out.sampler,
        };

        const auto writes = std::array< VkWriteDescriptorSet, 1 > {
            VkWriteDescriptorSet {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = out.descriptorSets[ i ],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .pBufferInfo = nullptr,
                .pImageInfo = &imageInfo,
                .pTexelBufferView = nullptr,
            },
        };

        vkUpdateDescriptorSets(
            getRenderDevice()->getHandle(),
            writes.size(),
            writes.data(),
            0,
            nullptr );
    }
}

void vulkan::RenderPass::destroyFramebufferAttachment( FramebufferAttachment &att ) const
{
    att.descriptorSets.clear();

    vkDestroyDescriptorSetLayout( getRenderDevice()->getHandle(), att.descriptorSetLayout, nullptr );
    att.descriptorSetLayout = VK_NULL_HANDLE;

    vkDestroyDescriptorPool( getRenderDevice()->getHandle(), att.descriptorPool, nullptr );
    att.descriptorPool = VK_NULL_HANDLE;

    vkDestroySampler( getRenderDevice()->getHandle(), att.sampler, nullptr );
    att.sampler = VK_NULL_HANDLE;
    vkDestroyImageView( getRenderDevice()->getHandle(), att.imageView, nullptr );
    att.imageView = VK_NULL_HANDLE;
    vkDestroyImage( getRenderDevice()->getHandle(), att.image, nullptr );
    att.image = VK_NULL_HANDLE;
    vkFreeMemory( getRenderDevice()->getHandle(), att.memory, nullptr );
    att.memory = VK_NULL_HANDLE;
}
