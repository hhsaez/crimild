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

#include "Rendering/VulkanCommandBuffer.hpp"

#include "Primitives/Primitive.hpp"
#include "Rendering/IndexBuffer.hpp"
#include "Rendering/VertexBuffer.hpp"
#include "Rendering/VulkanBuffer.hpp"
#include "Rendering/VulkanDescriptorSet.hpp"
#include "Rendering/VulkanFramebuffer.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderDeviceCache.hpp"
#include "Rendering/VulkanRenderPass.hpp"
#include "Rendering/VulkanRenderTarget.hpp"

#include <array>

using namespace crimild::vulkan;

CommandBuffer::CommandBuffer( RenderDevice *device, std::string name, VkCommandBufferLevel level ) noexcept
    : Named( name ),
      WithRenderDevice( device )
{
    auto allocInfo = VkCommandBufferAllocateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = getRenderDevice()->getCommandPool(),
        .level = level,
        .commandBufferCount = 1,
    };

    VkCommandBuffer handle;
    CRIMILD_VULKAN_CHECK(
        vkAllocateCommandBuffers(
            getRenderDevice()->getHandle(),
            &allocInfo,
            &handle
        )
    );
    setHandle( handle );

    getRenderDevice()->setObjectName( getHandle(), getName() );
}

CommandBuffer::~CommandBuffer( void ) noexcept
{
    auto handle = getHandle();
    vkFreeCommandBuffers(
        getRenderDevice()->getHandle(),
        getRenderDevice()->getCommandPool(),
        1,
        &handle
    );
    setHandle( VK_NULL_HANDLE );
}

void CommandBuffer::reset( void ) noexcept
{
    CRIMILD_VULKAN_CHECK(
        vkResetCommandBuffer(
            getHandle(),
            VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT
        )
    );

    m_boundObjects.clear();
}

void CommandBuffer::begin( VkCommandBufferUsageFlags flags ) noexcept
{
    // TODO: set pInheritanceInfo for secondary buffers
    auto beginInfo = VkCommandBufferBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = flags,
    };

    CRIMILD_VULKAN_CHECK(
        vkBeginCommandBuffer( getHandle(), &beginInfo )
    );
}

void CommandBuffer::invalidate( std::unordered_set< std::shared_ptr< Image > > &images ) noexcept
{
    for ( auto &image : images ) {
        const bool isColor = getRenderDevice()->formatIsColor( image->getFormat() );
        VkAccessFlags dstAccessMask = isColor ? VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT : ( VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT );

        const auto barrier = VkImageMemoryBarrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = 0,
            .dstAccessMask = dstAccessMask,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout =
                isColor
                    ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                    : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image->getHandle(),
            .subresourceRange = VkImageSubresourceRange {
                .aspectMask = image->getAspectFlags(),
                .baseMipLevel = 0,
                .levelCount = image->getMipLevels(),
                .baseArrayLayer = 0,
                .layerCount = image->getArrayLayers(),
            },
        };

        vkCmdPipelineBarrier(
            getHandle(),
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier
        );
    }
}

void CommandBuffer::beginRenderPass( std::shared_ptr< RenderPass > &renderPass, std::shared_ptr< Framebuffer > &framebuffer ) noexcept
{
    auto info = VkRenderPassBeginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderPass->getHandle(),
        .framebuffer = framebuffer->getHandle(),
        .renderArea = VkRect2D {
            .offset = { 0, 0 },
            .extent = framebuffer->getExtent(),
        },
        .clearValueCount = uint32_t( renderPass->getClearValues().size() ),
        .pClearValues = renderPass->getClearValues().data(),
    };

    vkCmdBeginRenderPass( getHandle(), &info, VK_SUBPASS_CONTENTS_INLINE );

    m_boundObjects.insert( renderPass );
    m_boundObjects.insert( framebuffer );
}

void CommandBuffer::bindPipeline( std::shared_ptr< GraphicsPipeline > &pipeline ) noexcept
{
    m_pipeline = pipeline->getHandle();
    m_pipelineLayout = pipeline->getPipelineLayout();
    m_pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    vkCmdBindPipeline(
        getHandle(),
        m_pipelineBindPoint,
        m_pipeline
    );

    m_boundObjects.insert( pipeline );
}
void CommandBuffer::bindDescriptorSet( uint32_t index, std::shared_ptr< DescriptorSet > &descriptorSet ) noexcept
{
    std::array< VkDescriptorSet, 1 > descriptorSets = { descriptorSet->getHandle() };
    vkCmdBindDescriptorSets(
        getHandle(),
        m_pipelineBindPoint,
        m_pipelineLayout,
        index,
        descriptorSets.size(),
        descriptorSets.data(),
        0,
        nullptr
    );

    m_boundObjects.insert( descriptorSet );

    descriptorSet->updateDescriptors();
}

void CommandBuffer::drawPrimitive( const std::shared_ptr< Primitive > &primitive ) noexcept
{
    auto cache = getRenderDevice()->getCache();

    primitive->getVertexData().each(
        [ &, i = 0 ]( auto &vertices ) mutable {
            if ( vertices != nullptr ) {
                auto buffer = cache->bind( vertices.get() );
                m_boundObjects.insert( buffer );
                VkBuffer buffers[] = { buffer->getHandle() };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers( getHandle(), i, 1, buffers, offsets );
            }
        }
    );

    auto indices = primitive->getIndices();
    if ( indices != nullptr ) {
        auto buffer = cache->bind( indices );
        m_boundObjects.insert( buffer );
        vkCmdBindIndexBuffer( getHandle(), buffer->getHandle(), 0, utils::getIndexType( indices ) );
        vkCmdDrawIndexed( getHandle(), indices->getIndexCount(), 1, 0, 0, 0 );
    } else {
        auto vertices = primitive->getVertexData()[ 0 ];
        if ( vertices != nullptr && vertices->getVertexCount() > 0 ) {
            vkCmdDraw( getHandle(), vertices->getVertexCount(), 1, 0, 0 );
        }
    }
}

void CommandBuffer::endRenderPass( void ) noexcept
{
    vkCmdEndRenderPass( getHandle() );
}

void CommandBuffer::flush( std::unordered_set< std::shared_ptr< Image > > &images ) noexcept
{
    for ( auto &image : images ) {
        const auto isColor = getRenderDevice()->formatIsColor( image->getFormat() );
        const auto barrier = VkImageMemoryBarrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
            .oldLayout =
                isColor
                    ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                    : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .newLayout =
                isColor
                    ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                    : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image->getHandle(),
            .subresourceRange = VkImageSubresourceRange {
                .aspectMask = image->getAspectFlags(),
                .baseMipLevel = 0,
                .levelCount = image->getMipLevels(),
                .baseArrayLayer = 0,
                .layerCount = image->getArrayLayers(),
            },
        };

        vkCmdPipelineBarrier(
            getHandle(),
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier
        );
    }
}

void CommandBuffer::end( void ) const noexcept
{
    CRIMILD_VULKAN_CHECK(
        vkEndCommandBuffer( getHandle() )
    );
}
