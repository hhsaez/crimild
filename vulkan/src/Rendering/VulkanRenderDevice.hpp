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

    class Light;
    class UniformBuffer;

    namespace vulkan {

        class CommandBuffer;
        class PhysicalDevice;
        class RenderDeviceCache;
        class Semaphore;
        class ShadowMapDEPRECATED;
        class VulkanSurface;

        class RenderDevice
            : public UniformBuffer::Observer,
              public VertexBuffer::Observer,
              public IndexBuffer::Observer,
              public DynamicSingleton< RenderDevice > {
        public:
            RenderDevice( PhysicalDevice *physicalDevice, VulkanSurface *surface, const Extent2D &extent ) noexcept;
            virtual ~RenderDevice( void ) noexcept;

            [[nodiscard]] inline const VkDevice &getHandle( void ) const noexcept { return m_handle; }

            [[nodiscard]] inline VkAllocationCallbacks *getAllocator( void ) const noexcept { return nullptr; }

            [[nodiscard]] inline const PhysicalDevice *getPhysicalDevice( void ) const noexcept { return m_physicalDevice; }

            void configure( uint32_t inFlightFrameCount ) noexcept;

            /**
             * \brief Get the total number of frames active at any given point in time
             */
            [[nodiscard]] inline uint32_t getInFlightFrameCount( void ) const noexcept { return m_inFlightFrameCount; }

            inline void setCurrentFrameIndex( uint8_t index ) noexcept { m_currentFrameIndex = index; }
            [[nodiscard]] inline uint8_t getCurrentFrameIndex( void ) const noexcept { return m_currentFrameIndex; }

            inline uint32_t getGraphicsQueueFamily( void ) const noexcept { return m_graphicsQueueFamily; }
            inline VkQueue getGraphicsQueue( void ) const noexcept { return m_graphicsQueueHandle; }
            inline uint32_t getComputeQueueFamily( void ) const noexcept { return m_computeQueueFamily; }
            inline VkQueue getComputeQueue( void ) const noexcept { return m_computeQueueHandle; }
            inline uint32_t getPresentQueueFamily( void ) const noexcept { return m_presentQueueFamily; }
            inline VkQueue getPresentQueue( void ) const noexcept { return m_presentQueueHandle; }

            inline ShaderCompiler &getShaderCompiler( void ) noexcept { return m_shaderCompiler; }

            void handle( const Event &e ) noexcept;

            inline void setObjectName( VkImage handle, std::string_view name ) const noexcept { setObjectName( UInt64( handle ), VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT, name ); }
            inline void setObjectName( VkImageView handle, std::string_view name ) const noexcept { setObjectName( UInt64( handle ), VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT, name ); }
            inline void setObjectName( VkSampler handle, std::string_view name ) const noexcept { setObjectName( UInt64( handle ), VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT, name ); }
            inline void setObjectName( VkCommandBuffer handle, std::string_view name ) const noexcept { setObjectName( UInt64( handle ), VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT, name ); }
            inline void setObjectName( VkDescriptorSetLayout handle, std::string_view name ) const noexcept { setObjectName( UInt64( handle ), VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT, name ); }
            inline void setObjectName( VkDescriptorSet handle, std::string_view name ) const noexcept { setObjectName( UInt64( handle ), VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT, name ); }
            inline void setObjectName( VkDescriptorPool handle, std::string_view name ) const noexcept { setObjectName( UInt64( handle ), VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT, name ); }
            inline void setObjectName( VkRenderPass handle, std::string_view name ) const noexcept { setObjectName( UInt64( handle ), VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT, name ); }
            inline void setObjectName( VkPipeline handle, std::string_view name ) const noexcept { setObjectName( UInt64( handle ), VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT, name ); }
            void setObjectName( UInt64 handle, VkDebugReportObjectTypeEXT objectType, std::string_view name ) const noexcept;

            [[nodiscard]] inline RenderDeviceCache *getCache( void ) noexcept
            {
                return m_caches[ getCurrentFrameIndex() ].get();
            }

            [[nodiscard]] inline RenderDeviceCache *getCache( uint32_t frameIndex ) noexcept
            {
                return m_caches[ frameIndex ].get();
            }

            void submitComputeCommands(
                std::shared_ptr< CommandBuffer > &commandBuffer,
                const std::vector< std::shared_ptr< Semaphore > > &wait = {},
                const std::vector< std::shared_ptr< Semaphore > > &signal = {}
            ) noexcept;

            void submitGraphicsCommands(
                std::shared_ptr< CommandBuffer > &commandBuffer,
                const std::vector< std::shared_ptr< Semaphore > > &wait = {},
                const std::vector< std::shared_ptr< Semaphore > > &signal = {}
            ) noexcept;

        private:
            void submitCommands(
                VkQueue queue,
                std::shared_ptr< CommandBuffer > &commandBuffer,
                const std::vector< std::shared_ptr< Semaphore > > &wait = {},
                const std::vector< std::shared_ptr< Semaphore > > &signal = {}
            ) noexcept;

        private:
            VkDevice m_handle = VK_NULL_HANDLE;
            PhysicalDevice *m_physicalDevice = nullptr;

            uint32_t m_graphicsQueueFamily = -1;
            VkQueue m_graphicsQueueHandle = VK_NULL_HANDLE;
            uint32_t m_computeQueueFamily = -1;
            VkQueue m_computeQueueHandle = VK_NULL_HANDLE;
            uint32_t m_presentQueueFamily = -1;
            VkQueue m_presentQueueHandle = VK_NULL_HANDLE;

            uint32_t m_inFlightFrameCount = 2;
            uint8_t m_currentFrameIndex = 0;

            ShaderCompiler m_shaderCompiler;

            std::vector< std::shared_ptr< RenderDeviceCache > > m_caches;

            /////////////////////////////////
            // MOSTLY DEPRECATED FROM HERE //
            /////////////////////////////////
        public:
            [[nodiscard]] inline const VulkanSurface *getSurface( void ) const noexcept { return m_surface; }

            // [[nodiscard]] inline const VkExtent2D &getSwapchainExtent( void ) const noexcept { return m_swapchainExtent; }
            // [[nodiscard]] inline const VkFormat &getSwapchainFormat( void ) const noexcept { return m_swapchainFormat; }
            // [[nodiscard]] inline const std::vector< SharedPointer< vulkan::ImageView > > &getSwapchainImageViews( void ) const noexcept { return m_swapchainImageViews; }
            // [[nodiscard]] inline size_t getSwapchainImageCount( void ) const noexcept { return m_swapchainImages.size(); }

            // [[nodiscard]] inline VkFormat getDepthStencilFormat( void ) const noexcept { return m_depthStencilResources.format; }
            // [[nodiscard]] inline const SharedPointer< vulkan::ImageView > &getDepthStencilImageView( void ) const noexcept { return m_depthStencilResources.imageView; }

            inline VkCommandPool getCommandPool( void ) const noexcept { return m_commandPool; }

            void flush( void ) noexcept;

            void createDescriptorSetLayout(
                const std::vector< VkDescriptorSetLayoutBinding > &bindings,
                VkDescriptorSetLayout &layout,
                std::string_view objectName = ""
            ) const noexcept;
            void destroyDescriptorSetLayout( VkDescriptorSetLayout &layout ) const noexcept;

            void createDescriptorPool(
                const std::vector< VkDescriptorPoolSize > &poolSizes,
                uint32_t additionalSets,
                VkDescriptorPool &descriptorPool,
                std::string_view objectName = ""
            ) const noexcept;
            void destroyDescriptorPool( VkDescriptorPool &descriptorPool ) const noexcept;

            void createSampler( const VkSamplerCreateInfo &createInfo, VkSampler &sampler ) const noexcept;
            [[deprecated]] void createSampler(
                const VkSamplerCreateInfo &createInfo,
                VkSampler &sampler,
                std::string_view objectName
            ) const noexcept;
            void destroySampler( VkSampler &sampler ) const noexcept;

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

            VkImage bind( const crimild::Image *image ) noexcept;
            void unbind( const crimild::Image *image ) noexcept;

            VkImageView bind( const crimild::ImageView *imageView ) noexcept;
            void unbind( const crimild::ImageView *imageView ) noexcept;

            VkSampler bind( const crimild::Sampler *sampler ) noexcept;
            void unbind( const crimild::Sampler *sampler ) noexcept;

            /**
             * \brief Get shadow map for a given light
             *
             * If the light does not cast shadows, nullptr will be returned.
             * Otherwise, it will return a valid ShadowMap pointer.
             *
             * A new ShadowMap instance will be created if needed.
             */
            ShadowMapDEPRECATED *getShadowMap( const Light *light ) noexcept;

            /**
             * \brief Get shadow map for a given light
             *
             * If the light does not cast shadows, nullptr will be returned.
             * Otherwise, it will return a valid ShadowMap pointer.
             *
             * Might return nullptr if the light was never bound before, since this is
             * a const function and has no side effect.
             */
            const ShadowMapDEPRECATED *getShadowMap( const Light *light ) const noexcept;

            // TODO: Move to RenderDeviceCache class instead. Retrieve using getRenderDevice()->getCache()->getImage(...);
            inline VkImage getImage( Size id, Index frameIndex ) const noexcept
            {
                return m_images.contains( id ) ? m_images.at( id )[ frameIndex ] : VK_NULL_HANDLE;
            }

            inline VkImageView getImageView( Size id, Index frameIndex ) const noexcept
            {
                return m_imageViews.contains( id ) ? m_imageViews.at( id )[ frameIndex ] : VK_NULL_HANDLE;
            }

            inline VkSampler getSampler( Size id, Index frameIndex ) const noexcept
            {
                return m_samplers.contains( id ) ? m_samplers.at( id )[ frameIndex ] : VK_NULL_HANDLE;
            }

            inline VkDescriptorSet getDescriptorSet( Size id, Index frameIndex ) const noexcept
            {
                return m_descriptorSets.contains( id ) ? m_descriptorSets.at( id )[ frameIndex ] : VK_NULL_HANDLE;
            }

            inline const VkDescriptorSet *getDescriptorSets( Size id ) const noexcept
            {
                return m_descriptorSets.contains( id ) ? m_descriptorSets.at( id ).data() : nullptr;
            }

            void createImage( const VkImageCreateInfo &createInfo, VkImage &image ) const noexcept;
            void destroyImage( VkImage &image ) const noexcept;

            void allocateImageMemory( const VkImage &image, VkDeviceMemory &imageMemory ) const noexcept;
            void allocateImageMemory( const VkImage &image, const VkMemoryAllocateInfo &memAllocInfo, VkDeviceMemory &imageMemory ) const noexcept;

            [[deprecated]] void createImage(
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

            void createImageView( const VkImageViewCreateInfo &createInfo, VkImageView &imageView ) const noexcept;

            [[deprecated]] void createImageView( VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView &imageView ) const noexcept;
            [[deprecated]] void createImageView( VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t baseArrayLayer, VkImageView &imageView ) const noexcept;
            [[deprecated]] void createImageViewArray( VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t layerCount, VkImageView &imageView ) const noexcept;

            [[nodiscard]] bool formatIsColor( VkFormat format ) const noexcept;
            [[nodiscard]] bool formatIsDepthStencil( VkFormat format ) const noexcept;
            [[nodiscard]] bool formatHasStencilComponent( VkFormat format ) const noexcept;

            void transitionImageLayout(
                VkCommandBuffer commandBuffer,
                VkImage image,
                VkFormat format,
                VkImageLayout oldLayout,
                VkImageLayout newLayout,
                crimild::UInt32 mipLevels,
                crimild::UInt32 layerCount,
                uint32_t baseArrayLayer = 0
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
                crimild::UInt32 layerCount,
                uint32_t baseArrayLayer = 0
            ) const noexcept;

            void generateMipmaps( VkImage image, VkFormat imageFormat, crimild::Int32 width, crimild::Int32 height, crimild::UInt32 mipLevels ) const noexcept;

            void createFramebufferAttachment(
                std::string name,
                const VkExtent2D &extent,
                VkFormat format,
                FramebufferAttachment &out,
                bool useDeviceImages = false
            );
            void destroyFramebufferAttachment( FramebufferAttachment &att );

            /**
             * \brief Flushes attachments contents and makes it read-only
             */
            void flush( const FramebufferAttachment &att ) const noexcept;

        private:
            void createCommandPool( VkCommandPool &commandPool ) noexcept;
            void destroyCommandPool( VkCommandPool &commandPool ) noexcept;

            VkCommandBuffer beginSingleTimeCommands( void ) const noexcept;
            void endSingleTimeCommands( VkCommandBuffer commandBuffer ) const noexcept;

        public:
            VulkanSurface *m_surface = nullptr;

            Extent2D m_extent;

            // VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
            // VkExtent2D m_swapchainExtent;
            // VkFormat m_swapchainFormat = VK_FORMAT_UNDEFINED;
            // std::vector< SharedPointer< vulkan::Image > > m_swapchainImages;
            // std::vector< SharedPointer< vulkan::ImageView > > m_swapchainImageViews;

            // std::vector< VkSemaphore > m_imageAvailableSemaphores;
            // std::vector< VkSemaphore > m_renderFinishedSemaphores;
            // std::vector< VkFence > m_inFlightFences;
            // std::vector< VkFence > m_imagesInFlight;

            VkCommandPool m_commandPool = VK_NULL_HANDLE;
            std::vector< VkCommandBuffer > m_commandBuffers;

            // Last image index provided by the swapchain
            // uint32_t m_imageIndex = 0;

            // TODO: Move these to RenderDeviceCache
            std::unordered_map< Size, std::vector< VkBuffer > > m_buffers;
            std::unordered_map< Size, std::vector< VkDeviceMemory > > m_memories;
            std::unordered_map< Size, std::vector< VkImage > > m_images;
            std::unordered_map< Size, std::vector< VkImageView > > m_imageViews;
            std::unordered_map< Size, std::vector< VkSampler > > m_samplers;
            std::unordered_map< Size, VkDescriptorPool > m_descriptorPools;
            std::unordered_map< Size, VkDescriptorSetLayout > m_descriptorSetLayouts;
            std::unordered_map< Size, std::vector< VkDescriptorSet > > m_descriptorSets;

            // struct DepthStencilResources {
            //     VkFormat format = VK_FORMAT_UNDEFINED;
            //     SharedPointer< vulkan::Image > image;
            //     SharedPointer< vulkan::ImageView > imageView;
            // } m_depthStencilResources;

            std::unordered_map< const Light *, SharedPointer< ShadowMapDEPRECATED > > m_shadowMaps;
            std::shared_ptr< ShadowMapDEPRECATED > m_fallbackDirectionalShadowMap;
            std::shared_ptr< ShadowMapDEPRECATED > m_fallbackPointShadowMap;
            std::shared_ptr< ShadowMapDEPRECATED > m_fallbackSpotShadowMap;
        };

    }

}

#endif
