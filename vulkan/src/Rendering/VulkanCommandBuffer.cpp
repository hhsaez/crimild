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
#include "Rendering/VulkanComputePipeline.hpp"
#include "Rendering/VulkanDescriptorSet.hpp"
#include "Rendering/VulkanFence.hpp"
#include "Rendering/VulkanFramebuffer.hpp"
#include "Rendering/VulkanGraphicsPipeline.hpp"
#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderDeviceCache.hpp"
#include "Rendering/VulkanRenderPass.hpp"
#include "Rendering/VulkanRenderTarget.hpp"

#include <array>

using namespace crimild::vulkan;

CommandBuffer::CommandBuffer( RenderDevice *device, std::string name, VkCommandBufferLevel level ) noexcept
    : Named( name ),
      WithRenderDevice( device ),
      m_fence( crimild::alloc< Fence >( device, name + "/Fence" ) )
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
    std::array< VkFence, 1 > fences = { m_fence->getHandle() };
    CRIMILD_VULKAN_CHECK(
        vkWaitForFences(
            getRenderDevice()->getHandle(),
            fences.size(),
            fences.data(),
            VK_TRUE,
            UINT64_MAX
        )
    );

    vkResetFences( getRenderDevice()->getHandle(), fences.size(), fences.data() );

    CRIMILD_VULKAN_CHECK(
        vkResetCommandBuffer(
            getHandle(),
            VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT
        )
    );

    m_boundObjects.clear();
}

void CommandBuffer::begin( SyncOptions const &options, VkCommandBufferUsageFlags flags ) noexcept
{
    // TODO: set pInheritanceInfo for secondary buffers
    auto beginInfo = VkCommandBufferBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = flags,
    };

    CRIMILD_VULKAN_CHECK(
        vkBeginCommandBuffer( getHandle(), &beginInfo )
    );

    for ( auto &barrier : options.pre.imageMemoryBarriers ) {
        pipelineBarrier( barrier );
    }
}

void CommandBuffer::pipelineBarrier( ImageMemoryBarrier const &info ) noexcept
{
    auto barrier = VkImageMemoryBarrier {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = info.srcAccessMask,
        .dstAccessMask = info.dstAccessMask,
        .oldLayout = info.oldLayout,
        .newLayout = info.newLayout,
        .srcQueueFamilyIndex = info.srcQueueFamily,
        .dstQueueFamilyIndex = info.dstQueueFamily,
        .image = info.imageView->getImage()->getHandle(),
        .subresourceRange = info.imageView->getSubresourceRange(),
    };

    vkCmdPipelineBarrier(
        getHandle(),
        info.srcStageMask,
        info.dstStageMask,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );

    m_boundObjects.insert( info.imageView );
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

void CommandBuffer::setViewport( const VkViewport &viewport ) noexcept
{
    vkCmdSetViewport( getHandle(), 0, 1, &viewport );
}

void CommandBuffer::setScissor( const VkRect2D &scissor ) noexcept
{
    vkCmdSetScissor( getHandle(), 0, 1, &scissor );
}

void CommandBuffer::setDepthBias( float constant, float clamp, float slope ) noexcept
{
    vkCmdSetDepthBias( getHandle(), constant, clamp, slope );
}

void CommandBuffer::bindPipeline( std::shared_ptr< ComputePipeline > &pipeline ) noexcept
{
    m_pipeline = pipeline->getHandle();
    m_pipelineLayout = pipeline->getPipelineLayout();
    m_pipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

    vkCmdBindPipeline(
        getHandle(),
        m_pipelineBindPoint,
        m_pipeline
    );

    m_boundObjects.insert( pipeline );
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

void CommandBuffer::draw( uint32_t count ) noexcept
{
    vkCmdDraw( getHandle(), count, 1, 0, 0 );
}

void CommandBuffer::drawPrimitive( const std::shared_ptr< Primitive > &primitive ) noexcept
{
    auto cache = getRenderDevice()->getCache();

    primitive->getVertexData().each(
        [ &, i = 0 ]( auto &vertices ) mutable {
            if ( vertices != nullptr ) {
                auto buffer = cache->bind( vertices );
                m_boundObjects.insert( buffer );
                VkBuffer buffers[] = { buffer->getHandle() };
                VkDeviceSize offsets[] = { 0 };
                vkCmdBindVertexBuffers( getHandle(), i, 1, buffers, offsets );
            }
        }
    );

    auto indices = primitive->getIndices();
    if ( indices != nullptr ) {
        auto buffer = cache->bind( retain( indices ) );
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

void CommandBuffer::transitionImageLayout( vulkan::Image *image, VkImageLayout newLayout ) const noexcept
{
    for ( uint32_t baseArrayLayer = 0; baseArrayLayer < image->getArrayLayers(); ++baseArrayLayer ) {
        // Transition all layers individually
        transitionImageLayout(
            image->getHandle(),
            image->getFormat(),
            image->getLayout(),
            newLayout,
            image->getMipLevels(),
            1,
            baseArrayLayer
        );
    }
    image->setLayout( newLayout );
}

void CommandBuffer::transitionImageLayout( vulkan::Image *image, VkImageLayout oldLayout, VkImageLayout newLayout ) const noexcept
{
    for ( uint32_t baseArrayLayer = 0; baseArrayLayer < image->getArrayLayers(); ++baseArrayLayer ) {
        // Transition all layers individually
        transitionImageLayout(
            image->getHandle(),
            image->getFormat(),
            oldLayout,
            newLayout,
            image->getMipLevels(),
            1,
            baseArrayLayer
        );
    }
    image->setLayout( newLayout );
}

void CommandBuffer::transitionImageLayout( VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, crimild::UInt32 mipLevels, crimild::UInt32 layerCount, uint32_t baseArrayLayer ) const noexcept
{
    auto barrier = VkImageMemoryBarrier {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = 0, // See below
        .dstAccessMask = 0, // See below
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = VkImageSubresourceRange {
            .aspectMask = 0, // Defined below
            .baseMipLevel = 0,
            .levelCount = mipLevels,
            .baseArrayLayer = baseArrayLayer,
            .layerCount = layerCount,
        },
    };

    VkPipelineStageFlags sourceStage = 0;
    VkPipelineStageFlags destinationStage = 0;

    if ( getRenderDevice()->formatIsDepthStencil( format ) ) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if ( getRenderDevice()->formatHasStencilComponent( format ) ) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_GENERAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL ) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) {
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT; // TODO
    } else if ( oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL ) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if ( oldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) {
        barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT; // TODO
    } else if ( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT; // TODO
    } else {
        std::stringstream ss;
        ss << "Unsupported VkImageLayout transition: " << oldLayout << " -> " << newLayout;
        CRIMILD_LOG_ERROR( ss.str() );
        CRIMILD_VULKAN_CHECK( VK_ERROR_FORMAT_NOT_SUPPORTED );
    }

    vkCmdPipelineBarrier(
        getHandle(),
        sourceStage,
        destinationStage,
        0,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );
}

void CommandBuffer::copy( const vulkan::Image *src, const vulkan::Image *dst, uint32_t dstBaseArrayLayer ) noexcept
{
    VkImageCopy copyRegion = {};
    copyRegion.srcSubresource.aspectMask = src->getAspectFlags();
    copyRegion.srcSubresource.baseArrayLayer = 0;
    copyRegion.srcSubresource.mipLevel = 0;
    copyRegion.srcSubresource.layerCount = 1;
    copyRegion.srcOffset = { 0, 0, 0 };
    copyRegion.dstSubresource.aspectMask = dst->getAspectFlags();
    copyRegion.dstSubresource.baseArrayLayer = dstBaseArrayLayer;
    copyRegion.dstSubresource.mipLevel = 0;
    copyRegion.dstSubresource.layerCount = 1;
    copyRegion.dstOffset = { 0, 0, 0 };
    copyRegion.extent = dst->getExtent();

    vkCmdCopyImage(
        getHandle(),
        src->getHandle(),
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        dst->getHandle(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &copyRegion
    );
}

void CommandBuffer::copy(
    std::shared_ptr< vulkan::Buffer > const &src,
    std::shared_ptr< vulkan::ImageView > const &dst
) noexcept
{
    const auto region = VkBufferImageCopy {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = dst->getSubresourceRange().aspectMask,
            .mipLevel = dst->getSubresourceRange().baseMipLevel,
            .baseArrayLayer = dst->getSubresourceRange().baseArrayLayer,
            .layerCount = dst->getSubresourceRange().layerCount,
        },
        .imageOffset = { 0, 0 },
        .imageExtent = dst->getImage()->getExtent(),
    };

    vkCmdCopyBufferToImage(
        getHandle(),
        src->getHandle(),
        dst->getImage()->getHandle(),
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    m_boundObjects.insert( src );
    m_boundObjects.insert( dst );
}

void CommandBuffer::dispatch( uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ ) noexcept
{
    vkCmdDispatch( getHandle(), groupCountX, groupCountY, groupCountZ );
}

void CommandBuffer::end( SyncOptions const &options ) noexcept
{
    for ( auto &barrier : options.post.imageMemoryBarriers ) {
        pipelineBarrier( barrier );
    }

    CRIMILD_VULKAN_CHECK(
        vkEndCommandBuffer( getHandle() )
    );
}
