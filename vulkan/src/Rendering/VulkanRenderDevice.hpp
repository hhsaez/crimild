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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_DEVICE_
#define CRIMILD_VULKAN_RENDERING_RENDER_DEVICE_

#include "Foundation/VulkanUtils.hpp"
#include "Rendering/IndexBuffer.hpp"
#include "Rendering/UniformBuffer.hpp"
#include "Rendering/VertexBuffer.hpp"
#include "Rendering/VulkanFramebufferAttachment.hpp"
#include "Rendering/VulkanShaderCompiler.hpp"

#include <unordered_map>
#include <vector>

namespace crimild {

    struct Event;

    class UniformBuffer;

    namespace vulkan {

        class PhysicalDevice;
        class VulkanSurface;

        class RenderDevice
            : public UniformBuffer::Observer,
              public VertexBuffer::Observer,
              public IndexBuffer::Observer {
        public:
            RenderDevice( PhysicalDevice *physicalDevice, VulkanSurface *surface, const Extent2D &extent ) noexcept;
            virtual ~RenderDevice( void ) noexcept;

            [[nodiscard]] inline const VkDevice &getHandle( void ) const noexcept { return m_handle; }

            [[nodiscard]] inline const PhysicalDevice *getPhysicalDevice( void ) const noexcept { return m_physicalDevice; }
            [[nodiscard]] inline const VulkanSurface *getSurface( void ) const noexcept { return m_surface; }

            [[nodiscard]] inline const VkExtent2D &getSwapchainExtent( void ) const noexcept { return m_swapchainExtent; }
            [[nodiscard]] inline const VkFormat &getSwapchainFormat( void ) const noexcept { return m_swapchainFormat; }
            [[nodiscard]] inline const std::vector< VkImageView > &getSwapchainImageViews( void ) const noexcept { return m_swapchainImageViews; }
            [[nodiscard]] inline size_t getSwapchainImageCount( void ) const noexcept { return m_swapchainImages.size(); }

            [[nodiscard]] inline VkFormat getDepthStencilFormat( void ) const noexcept { return m_depthStencilResources.format; }
            [[nodiscard]] inline VkImageView getDepthStencilImageView( void ) const noexcept { return m_depthStencilResources.imageView; }

            [[nodiscard]] inline uint8_t getCurrentFrameIndex( void ) const noexcept { return m_imageIndex; }
            [[nodiscard]] inline VkCommandBuffer getCurrentCommandBuffer( void ) const noexcept { return m_commandBuffers[ m_imageIndex ]; }

            void handle( const Event &e ) noexcept;

            bool beginRender( void ) noexcept;
            bool endRender( void ) noexcept;

            void flush( void ) noexcept;

            inline ShaderCompiler &getShaderCompiler( void ) noexcept { return m_shaderCompiler; }

            using UniformBuffer::Observer::ignore;
            using UniformBuffer::Observer::observe;
            bool bind( const UniformBuffer *uniformBuffer ) noexcept;
            void unbind( const UniformBuffer *uniformBuffer ) noexcept;
            virtual void onDestroy( const UniformBuffer *uniformBuffer ) noexcept override
            {
                unbind( uniformBuffer );
            }
            VkBuffer getHandle( UniformBuffer *uniformBuffer, Index imageIndex ) const noexcept;
            void update( UniformBuffer *uniformBuffer ) const noexcept;

            using VertexBuffer::Observer::ignore;
            using VertexBuffer::Observer::observe;
            VkBuffer bind( const VertexBuffer *vertexBuffer ) noexcept;
            void unbind( const VertexBuffer *vertexBuffer ) noexcept;
            virtual void onDestroy( const VertexBuffer *vertexBuffer ) noexcept override
            {
                unbind( vertexBuffer );
            }

            using IndexBuffer::Observer::ignore;
            using IndexBuffer::Observer::observe;
            VkBuffer bind( const IndexBuffer *indexBuffer ) noexcept;
            void unbind( const IndexBuffer *indexBuffer ) noexcept;
            virtual void onDestroy( const IndexBuffer *indexBuffer ) noexcept override
            {
                unbind( indexBuffer );
            }

            VkImage bind( const Image *image ) noexcept;
            void unbind( const Image *image ) noexcept;

            VkImageView bind( const ImageView *imageView ) noexcept;
            void unbind( const ImageView *imageView ) noexcept;

            VkSampler bind( const Sampler *sampler ) noexcept;
            void unbind( const Sampler *sampler ) noexcept;

            inline void setObjectName( VkImage handle, std::string name ) const noexcept { setObjectName( UInt64( handle ), VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT, name ); }
            inline void setObjectName( VkImageView handle, std::string name ) const noexcept { setObjectName( UInt64( handle ), VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT, name ); }
            inline void setObjectName( VkSampler handle, std::string name ) const noexcept { setObjectName( UInt64( handle ), VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT, name ); }
            inline void setObjectName( VkDescriptorSetLayout handle, std::string name ) const noexcept { setObjectName( UInt64( handle ), VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT, name ); }
            inline void setObjectName( VkDescriptorPool handle, std::string name ) const noexcept { setObjectName( UInt64( handle ), VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT, name ); }
            void setObjectName( UInt64 handle, VkDebugReportObjectTypeEXT objectType, std::string name ) const noexcept;

            void createImage(
                crimild::UInt32 width,
                crimild::UInt32 height,
                VkFormat format,
                VkImageTiling tiling,
                VkImageUsageFlags usage,
                VkMemoryPropertyFlags memoryProperties,
                crimild::UInt32 mipLevels,
                VkSampleCountFlagBits numSamples,
                crimild::UInt32 arrayLayers,
                crimild::UInt32 flags,
                VkImage &image,
                VkDeviceMemory &imageMemory,
                void *imageData = nullptr
            ) const noexcept;

            void createImageView( VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView &imageView ) const noexcept;

            [[nodiscard]] bool formatIsColor( VkFormat format ) const;
            [[nodiscard]] bool formatIsDepthStencil( VkFormat format ) const;

            void transitionImageLayout(
                VkCommandBuffer commandBuffer,
                VkImage image,
                VkFormat format,
                VkImageLayout oldLayout,
                VkImageLayout newLayout,
                crimild::UInt32 mipLevels,
                crimild::UInt32 layerCount
            ) const noexcept;

            VkViewport getViewport( const ViewportDimensions &viewport ) const noexcept;
            VkRect2D getScissor( const ViewportDimensions &scissor ) const noexcept;

            void createBuffer(
                VkDeviceSize size,
                VkBufferUsageFlags usage,
                VkMemoryPropertyFlags properties,
                VkBuffer &bufferHandler,
                VkDeviceMemory &bufferMemory
            ) const noexcept;

            void copyToBuffer( VkDeviceMemory &bufferMemory, const void *data, VkDeviceSize size ) const noexcept;

            void copyBufferToImage( VkBuffer buffer, VkImage image, crimild::UInt32 width, crimild::UInt32 height, UInt32 layerCount ) const noexcept;

            void transitionImageLayout(
                VkImage image,
                VkFormat format,
                VkImageLayout oldLayout,
                VkImageLayout newLayout,
                crimild::UInt32 mipLevels,
                crimild::UInt32 layerCount
            ) const noexcept;

            void generateMipmaps( VkImage image, VkFormat imageFormat, crimild::Int32 width, crimild::Int32 height, crimild::UInt32 mipLevels ) const noexcept;

            void createFramebufferAttachment(
                std::string name,
                const VkExtent2D &extent,
                VkFormat format,
                FramebufferAttachment &out,
                bool useDeviceImages = false
            ) const;
            void destroyFramebufferAttachment( FramebufferAttachment &att ) const;

            /**
             * \brief Flushes attachments contents and makes it read-only
             */
            void flush( const FramebufferAttachment &att ) const noexcept;

        private:
            void createSwapchain( void ) noexcept;
            void destroySwapchain( void ) noexcept;

            void createDepthStencilResources( void ) noexcept;
            void destroyDepthStencilResources( void ) noexcept;

            void createSyncObjects( void ) noexcept;
            void destroySyncObjects( void ) noexcept;

            void createCommandPool( VkCommandPool &commandPool ) noexcept;
            void destroyCommandPool( VkCommandPool &commandPool ) noexcept;

            void createCommandBuffers( void ) noexcept;
            void destroyCommandBuffers( void ) noexcept;
            void createCommandBuffer( VkCommandBuffer &commandBuffer ) noexcept;
            void destroyCommandBuffer( VkCommandBuffer &commandBuffer ) noexcept;

            VkCommandBuffer beginSingleTimeCommands( void ) const noexcept;
            void endSingleTimeCommands( VkCommandBuffer commandBuffer ) const noexcept;

        private:
            // TODO(hernan): rename to m_device
            VkDevice m_handle = VK_NULL_HANDLE;
            PhysicalDevice *m_physicalDevice = nullptr;

            VulkanSurface *m_surface = nullptr;

            Extent2D m_extent;

            VkQueue m_graphicsQueueHandle = VK_NULL_HANDLE;
            VkQueue m_computeQueueHandle = VK_NULL_HANDLE;
            VkQueue m_presentQueueHandle = VK_NULL_HANDLE;

            VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
            VkExtent2D m_swapchainExtent;
            VkFormat m_swapchainFormat = VK_FORMAT_UNDEFINED;
            std::vector< VkImage > m_swapchainImages;
            std::vector< VkImageView > m_swapchainImageViews;

            std::vector< VkSemaphore > m_imageAvailableSemaphores;
            std::vector< VkSemaphore > m_renderFinishedSemaphores;
            std::vector< VkFence > m_inFlightFences;
            std::vector< VkFence > m_imagesInFlight;

            VkCommandPool m_commandPool = VK_NULL_HANDLE;
            std::vector< VkCommandBuffer > m_commandBuffers;

            // Last image index provided by the swapchain
            uint32_t m_imageIndex = 0;

            // To use the right pair of semaphores every time, we also keep track of the current frame
            uint8_t m_currentFrame = 0;

            ShaderCompiler m_shaderCompiler;

            std::unordered_map< Size, std::vector< VkBuffer > > m_buffers;
            std::unordered_map< Size, std::vector< VkDeviceMemory > > m_memories;
            std::unordered_map< Size, std::vector< VkImage > > m_images;
            std::unordered_map< Size, std::vector< VkImageView > > m_imageViews;
            std::unordered_map< Size, std::vector< VkSampler > > m_samplers;

            struct DepthStencilResources {
                VkFormat format = VK_FORMAT_UNDEFINED;
                VkImage image = VK_NULL_HANDLE;
                VkDeviceMemory memory = VK_NULL_HANDLE;
                VkImageView imageView = VK_NULL_HANDLE;
            } m_depthStencilResources;
        };

    }

}

#endif
