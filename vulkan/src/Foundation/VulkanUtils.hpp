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

#include "Foundation/VulkanInitializers.hpp"

#include <Crimild.hpp>
#include <Crimild_Foundation.hpp>
#include <vulkan/vulkan.h>

namespace crimild {

    class IndexBuffer;
    struct ViewportDimensions;

    namespace vulkan {

        class Buffer;
        class CommandBuffer;
        class Image;
        class RenderDevice;
        class RenderTarget;

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
            VkIndexType getIndexType( const IndexBuffer *indexBuffer ) noexcept;
            VkCompareOp getCompareOp( const CompareOp &compareOp ) noexcept;
            VkSamplerAddressMode getSamplerAddressMode( Texture::WrapMode wrapMode ) noexcept; //< Deprecated
            VkSamplerAddressMode getSamplerAddressMode( crimild::Sampler::WrapMode wrapMode ) noexcept;
            VkBorderColor getBorderColor( Texture::BorderColor borderColor ) noexcept; //< Deprecated
            VkBorderColor getBorderColor( crimild::Sampler::BorderColor borderColor ) noexcept;
            VkFilter getSamplerFilter( crimild::Sampler::Filter filter ) noexcept;
            VkFormat getFormat( Format format ) noexcept;
            Format getFormat( VkFormat format ) noexcept; //< Reversed
            crimild::Bool formatIsColor( Format format ) noexcept;
            crimild::Bool formatIsDepthStencil( Format format ) noexcept;
            VkImageUsageFlags getAttachmentUsage( Attachment::Usage usage ) noexcept;
            Extent2D getExtent( const VkExtent2D &extent ) noexcept; //< Reverse
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
            VkSurfaceFormatKHR chooseSurfaceFormat( const std::vector< VkSurfaceFormatKHR > &availableFormats ) noexcept;
            VkExtent2D chooseExtent( const VkSurfaceCapabilitiesKHR &capabilities, VkExtent2D requestedExtent ) noexcept;

            /**
             * \brief Choose the best presentation mode from the available ones
             *
             * By default, it will attempt to use VK_PRESENT_MODE_MAILBOX_KHR, which can be considered as
             * triple buffer and vsync enabled. Otherwhise, it will fallback to use VK_PRESENT_MODE_FIFO_KHR which,
             * accoriding to the standard, it's the only mode that is mandatory to be supported but it could
             * introduce some latency (but no tearing).
             *
             * If we want to disable vsync, we should use VK_PRESENT_MODE_IMMEDIATE_KHR if available.
             */
            VkPresentModeKHR choosePresentationMode( const std::vector< VkPresentModeKHR > &availablePresentModes ) noexcept;

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

            crimild::Bool copyToBuffer( const VkDevice &device, VkDeviceMemory &bufferMemory, const void *data, VkDeviceSize size ) noexcept;
            crimild::Bool copyToBuffer( const VkDevice &device, VkDeviceMemory &bufferMemory, const void **data, crimild::UInt32 count, VkDeviceSize size ) noexcept;

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
                VkMemoryPropertyFlags memoryProperties = VK_MEMORY_HEAP_DEVICE_LOCAL_BIT;
                crimild::UInt32 mipLevels = 1;
                VkSampleCountFlagBits numSamples = VK_SAMPLE_COUNT_1_BIT;
                crimild::UInt32 arrayLayers = 1;
                crimild::UInt32 flags = 0;
            };

            VkImageType getImageType( crimild::Image *image ) noexcept;
            VkImageAspectFlags getImageAspectFlags( crimild::Image *image ) noexcept;

            //@}

            /**
                           \name ImageView
                        */
            //@{

            VkImageViewType getImageViewType( const ImageView *imageView ) noexcept;
            VkImageAspectFlags getImageViewAspectFlags( const ImageView *imageView ) noexcept;

            [[deprecated]] void createImageView( VkDevice renderDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView *imageView ) noexcept;

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

            crimild::Bool hasStencilComponent( VkFormat format ) noexcept;

            //@}

            /**
                \name Debug
             */
            //@{

            void populateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT &createInfo ) noexcept;

            void setObjectName( VkDevice device, UInt64 object, VkDebugReportObjectTypeEXT objectType, const char *name ) noexcept;

            //@}

        }

        class WithRenderDevice {
        public:
            WithRenderDevice( RenderDevice *rd ) noexcept
                : m_renderDevice( rd )
            {
                // no-op
            }

            virtual ~WithRenderDevice( void ) noexcept
            {
                m_renderDevice = nullptr;
            }

            [[nodiscard]] inline RenderDevice *getRenderDevice( void ) noexcept { return m_renderDevice; }
            [[nodiscard]] inline const RenderDevice *getRenderDevice( void ) const noexcept { return m_renderDevice; }

        private:
            RenderDevice *m_renderDevice = nullptr;
        };

        template< typename HandleType >
        class WithHandle {
        public:
            virtual ~WithHandle( void ) noexcept
            {
                m_handle = VK_NULL_HANDLE;
            }

            [[nodiscard]] inline HandleType getHandle( void ) const noexcept { return m_handle; }

        protected:
            inline void setHandle( HandleType handle ) noexcept { m_handle = handle; }

        private:
            HandleType m_handle = VK_NULL_HANDLE;
        };

        class WithCommandBuffer {
        protected:
            WithCommandBuffer( std::shared_ptr< CommandBuffer > const &commandBuffer )
                : m_commandBuffer( commandBuffer )
            {
                // no-op
            }

        public:
            virtual ~WithCommandBuffer( void ) = default;

            inline std::shared_ptr< CommandBuffer > &getCommandBuffer( void ) noexcept { return m_commandBuffer; }

        private:
            std::shared_ptr< CommandBuffer > m_commandBuffer;
        };

    }

}

std::ostream &operator<<( std::ostream &out, VkImageLayout layout ) noexcept;

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
