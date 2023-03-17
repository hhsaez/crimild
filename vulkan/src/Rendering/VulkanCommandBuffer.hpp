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

#ifndef CRIMILD_VULKAN_RENDERING_COMMAND_BUFFER_
#define CRIMILD_VULKAN_RENDERING_COMMAND_BUFFER_

#include "Foundation/Named.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/VulkanUtils.hpp"

namespace crimild {

    class Primitive;

}

namespace crimild::vulkan {

    class ComputePipeline;
    class DescriptorSet;
    class Framebuffer;
    class GraphicsPipeline;
    class Image;
    class RenderPass;

    /**
     * A command buffer will retain ownership of all bound resources in between begin/end calls.
     * This is to prevent removing resources while some buffers are still being executed.
     *
     * \remarks CommandBuffer is not thread-safe. When recording command from multiple threads, it
     * is recommended to create a secondary-level command buffer for each thread and then execute them
     * together in a primary level one.
     */
    class CommandBuffer
        : public SharedObject,
          public Named,
          public WithRenderDevice,
          public WithHandle< VkCommandBuffer > {
    public:
        CommandBuffer(
            RenderDevice *device,
            std::string name,
            VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY
        ) noexcept;

        virtual ~CommandBuffer( void ) noexcept;

        void reset( void ) noexcept;

        void begin( VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT ) noexcept;

        void invalidate( std::unordered_set< std::shared_ptr< Image > > &images ) noexcept;

        void beginRenderPass( std::shared_ptr< RenderPass > &renderPass, std::shared_ptr< Framebuffer > &framebuffer ) noexcept;

        void setViewport( const VkViewport &viewport ) noexcept;
        void setScissor( const VkRect2D &scissor ) noexcept;

        void setDepthBias( float constant, float clamp, float slope ) noexcept;

        void bindPipeline( std::shared_ptr< ComputePipeline > &pipeline ) noexcept;
        void bindPipeline( std::shared_ptr< GraphicsPipeline > &pipeline ) noexcept;

        void bindDescriptorSet( uint32_t index, std::shared_ptr< DescriptorSet > &descriptorSet ) noexcept;

        template< typename ConstantType >
        void pushConstants( VkShaderStageFlags stage, uint32_t index, const ConstantType &value ) noexcept
        {
            // Vulkan spec only requires a minimum of 128 bytes. Anything larger should
            // use normal uniforms instead.
            vkCmdPushConstants(
                getHandle(),
                m_pipelineLayout,
                stage,
                0,
                sizeof( ConstantType ),
                &value
            );
        }

        void draw( uint32_t count ) noexcept;
        void drawPrimitive( const std::shared_ptr< Primitive > &primitive ) noexcept;

        void endRenderPass( void ) noexcept;

        void flush( std::unordered_set< std::shared_ptr< Image > > &images ) noexcept;

        void transitionImageLayout( vulkan::Image *image, VkImageLayout newLayout ) const noexcept;
        void transitionImageLayout( vulkan::Image *image, VkImageLayout oldLayout, VkImageLayout newLayout ) const noexcept;

        void copy( const vulkan::Image *src, const vulkan::Image *dst, uint32_t dstBaseArrayLayer = 0 ) noexcept;

        void dispatch( uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ ) noexcept;

        void end( void ) const noexcept;

    private:
        void transitionImageLayout( VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, crimild::UInt32 mipLevels, crimild::UInt32 layerCount, uint32_t baseArrayLayer = 0 ) const noexcept;

    private:
        std::unordered_set< std::shared_ptr< SharedObject > > m_boundObjects;

        VkPipeline m_pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
        VkPipelineBindPoint m_pipelineBindPoint;
    };

}

#endif
