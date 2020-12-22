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

#ifndef CRIMILD_VULKAN_UTILS_
#define CRIMILD_VULKAN_UTILS_

#include "Exceptions/VulkanException.hpp"
#include "Foundation/Log.hpp"
#include "Foundation/Types.hpp"
#include "Rendering/DescriptorSet.hpp"
#include "Rendering/Format.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Texture.hpp"

#include <vulkan/vulkan.h>

namespace crimild {

    class IndexBuffer;
    struct ViewportDimensions;

    namespace vulkan {

        class RenderDevice;

        namespace utils {

            static const VkPrimitiveTopology VULKAN_PRIMITIVE_TOPOLOGIES[] = {
                VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
                VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
                VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
                VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
                VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
                VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,
            };

            static const VkCommandBufferUsageFlagBits VULKAN_COMMAND_BUFFER_USAGE[] = {
                VkCommandBufferUsageFlagBits(),
                VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
                VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
            };

            const char *errorToString( VkResult result ) noexcept;

            /**
             	\name Translation
             */
            //@{

            VkShaderStageFlagBits getVulkanShaderStageFlag( Shader::Stage stage ) noexcept;
            VkDescriptorType getVulkanDescriptorType( DescriptorType type ) noexcept;
            VkFilter getVulkanFilter( Texture::Filter filter ) noexcept; //< Deprecated
            VkRect2D getViewportRect( const ViewportDimensions *viewport, const RenderDevice *renderDevice ) noexcept;
            VkViewport getViewport( const ViewportDimensions *viewport, const RenderDevice *renderDevice ) noexcept;
            VkRect2D getScissor( const ViewportDimensions *scissor, const RenderDevice *renderDevice ) noexcept;
            VkIndexType getIndexType( const IndexBuffer *indexBuffer ) noexcept;
            VkCompareOp getCompareOp( const CompareOp &compareOp ) noexcept;
            VkSamplerAddressMode getSamplerAddressMode( Texture::WrapMode wrapMode ) noexcept; //< Deprecated
            VkSamplerAddressMode getSamplerAddressMode( Sampler::WrapMode wrapMode ) noexcept;
            VkBorderColor getBorderColor( Texture::BorderColor borderColor ) noexcept; //< Deprecated
            VkBorderColor getBorderColor( Sampler::BorderColor borderColor ) noexcept;
            VkFilter getSamplerFilter( Sampler::Filter filter ) noexcept;
            VkFormat getFormat( RenderDevice *renderDevice, Format format ) noexcept;
            Format getFormat( VkFormat format ) noexcept; //< Reversed
            crimild::Bool formatIsColor( Format format ) noexcept;
            crimild::Bool formatIsDepthStencil( Format format ) noexcept;
            VkImageUsageFlags getAttachmentUsage( Attachment::Usage usage ) noexcept;
            VkExtent2D getExtent( Extent2D extent, const RenderDevice *renderDevice ) noexcept;
            VkAttachmentLoadOp getLoadOp( Attachment::LoadOp loadOp ) noexcept;
            VkAttachmentStoreOp getStoreOp( Attachment::StoreOp storeOp ) noexcept;

            //@}

            /**
             	\name Validation layers
             */
            //{

            /**
               \brief Used for checking if validation layers should be enabled
             */
            crimild::Bool checkValidationLayersEnabled( void ) noexcept;

            using ValidationLayerArray = std::vector< const char * >;

            /**
               \brief The list of validation layers (only valid if they're enabled)
               \see areValidationLayersEnabled()
             */
            const ValidationLayerArray &getValidationLayers( void ) noexcept;

            crimild::Bool checkValidationLayerSupport( const ValidationLayerArray &validationLayers ) noexcept;

            //@}

            /**
             	\name Extensions
             */
            //@{

            using ExtensionArray = std::vector< const char * >;

            ExtensionArray getRequiredExtensions( void ) noexcept;

            const ExtensionArray &getDeviceExtensions( void ) noexcept;

            /**
             	\brief Check if a given device meets all of the required extensions
             */
            crimild::Bool checkDeviceExtensionSupport( const VkPhysicalDevice &device ) noexcept;

            //@}

            /**
             	\name Queue family queries
             */
            //@{

            struct QueueFamilyIndices {
                std::vector< crimild::UInt32 > graphicsFamily;
                std::vector< crimild::UInt32 > computeFamily;
                std::vector< crimild::UInt32 > presentFamily;

                bool isComplete( void ) const noexcept
                {
                    return graphicsFamily.size() > 0
                           && computeFamily.size() > 0
                           && presentFamily.size() > 0;
                }
            };

            // TODO: How to handle an optional surface param?
            QueueFamilyIndices findQueueFamilies( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept;

            //@}

            /**
             	\name Swapchain
             */
            //@{

            struct SwapchainSupportDetails {
                VkSurfaceCapabilitiesKHR capabilities;
                std::vector< VkSurfaceFormatKHR > formats;
                std::vector< VkPresentModeKHR > presentModes;
            };

            SwapchainSupportDetails querySwapchainSupportDetails( const VkPhysicalDevice &device, const VkSurfaceKHR &surface ) noexcept;
            crimild::Bool checkSwapchainSupport( const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface ) noexcept;

            //@}

            /**
             	\name Buffers
             */
            //@{

            crimild::UInt32 findMemoryType( const VkPhysicalDevice &physicalDevice, crimild::UInt32 typeFilter, VkMemoryPropertyFlags properties ) noexcept;

            struct BufferDescriptor {
                VkDeviceSize size;
                VkBufferUsageFlags usage;
                VkMemoryPropertyFlags properties;
            };

            crimild::Bool createBuffer( RenderDevice *renderDevice, BufferDescriptor const &descriptor, VkBuffer &bufferHandler, VkDeviceMemory &bufferMemory ) noexcept;

            crimild::Bool copyToBuffer( const VkDevice &device, VkDeviceMemory &bufferMemory, const void *data, VkDeviceSize size ) noexcept;
            crimild::Bool copyToBuffer( const VkDevice &device, VkDeviceMemory &bufferMemory, const void **data, crimild::UInt32 count, VkDeviceSize size ) noexcept;

            crimild::Bool copyBuffer( RenderDevice *renderDevice, VkBuffer src, VkBuffer dst, VkDeviceSize size ) noexcept;

            //@}

            /**
             	\name Images
             */
            //@{

            struct ImageDescriptor {
                crimild::UInt32 width;
                crimild::UInt32 height;
                VkFormat format;
                VkImageTiling tiling;
                VkImageUsageFlags usage;
                VkMemoryPropertyFlags properties;
                crimild::UInt32 mipLevels = 1;
                VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT;
                crimild::UInt32 arrayLayers = 1;
                crimild::UInt32 flags = 0;
            };

            crimild::Bool createImage( RenderDevice *renderDevice, ImageDescriptor const &descriptor, VkImage &image, VkDeviceMemory &imageMemory ) noexcept;

            void transitionImageLayout( RenderDevice *renderDevice, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, crimild::UInt32 mipLevels, crimild::UInt32 layerCount ) noexcept;

            void copyBufferToImage( RenderDevice *renderDevice, VkBuffer buffer, VkImage image, crimild::UInt32 width, crimild::UInt32 height, UInt32 layerCount ) noexcept;
            void copyBufferToLayeredImage( RenderDevice *renderDevice, VkBuffer buffer, VkImage image, crimild::Size layerCount, crimild::Size layerSize, crimild::UInt32 layerWidth, crimild::UInt32 layerHeight ) noexcept;

            void generateMipmaps( RenderDevice *renderDevice, VkImage image, VkFormat format, crimild::Int32 width, crimild::Int32 height, crimild::UInt32 mipLevels ) noexcept;

            VkImageType getImageType( Image *image ) noexcept;
            VkImageAspectFlags getImageAspectFlags( Image *image ) noexcept;

            //@}

            /**
			   \name ImageView
			*/
            //@{

            VkImageViewType getImageViewType( ImageView *imageView ) noexcept;
            VkFormat getImageViewFormat( RenderDevice *renderDevice, ImageView *imageView ) noexcept;
            VkImageAspectFlags getImageViewAspectFlags( ImageView *imageView ) noexcept;

            //@}

            /**
             	\name Multisampling
             */
            //@{

            VkSampleCountFlagBits getMaxUsableSampleCount( VkPhysicalDevice physicalDevice ) noexcept;

            //@}

            /**
             	\name Depth and Stencil
             */
            //@{

            VkFormat findSupportedFormat( RenderDevice *renderDevice, const std::vector< VkFormat > &candidates, VkImageTiling tiling, VkFormatFeatureFlags features ) noexcept;

            VkFormat findDepthFormat( RenderDevice *renderDevice ) noexcept;

            crimild::Bool hasStencilComponent( VkFormat format ) noexcept;

            //@}

            /**
             	\name Commands
             */
            //@{

            VkCommandBuffer beginSingleTimeCommands( RenderDevice *renderDevice ) noexcept;
            void endSingleTimeCommands( RenderDevice *renderDevice, VkCommandBuffer commandBuffer ) noexcept;

            //@}

            /**
             	\name Debug
             */
            //@{

            void populateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT &createInfo ) noexcept;

            void setObjectName( VkDevice device, UInt64 object, VkDebugReportObjectTypeEXT objectType, const char *name ) noexcept;

            //@}

        }

    }

}

#define CRIMILD_VULKAN_CHECK( x )                                         \
    {                                                                     \
        VkResult ret = x;                                                 \
        if ( ret != VK_SUCCESS ) {                                        \
            auto errorStr = crimild::vulkan::utils::errorToString( ret ); \
            std::cerr << "Vulkan Error:"                                  \
                      << "\n\tFile: " << __FILE__                         \
                      << "\n\tLine: " << __LINE__                         \
                      << "\n\tResult: " << errorStr                       \
                      << "\n\tCaller: " << #x;                            \
            exit( -1 );                                                   \
        }                                                                 \
    }

#endif
