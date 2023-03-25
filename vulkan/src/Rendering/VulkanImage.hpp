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

#ifndef CRIMILD_VULKAN_RENDERING_IMAGE_
#define CRIMILD_VULKAN_RENDERING_IMAGE_

#include "Foundation/Named.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/VulkanUtils.hpp"
#include "Rendering/VulkanWithRenderDeviceDEPRECATED.hpp"

namespace crimild {

    namespace vulkan {

        class Image
            : public SharedObject,
              public Named,
              public WithConstRenderDevice {
        public:
            static auto createInfo( void ) noexcept
            {
                return VkImageCreateInfo {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                    .flags = 0,
                    .imageType = VK_IMAGE_TYPE_2D,
                    .mipLevels = 1,
                    .arrayLayers = 1,
                    .samples = VK_SAMPLE_COUNT_1_BIT,
                    .tiling = VK_IMAGE_TILING_OPTIMAL,
                    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                    .initialLayout = VK_IMAGE_LAYOUT_GENERAL,
                };
            }

        public:
            Image(
                const RenderDevice *device,
                const VkImageCreateInfo &createInfo,
                std::string name = "Image"
            ) noexcept;

            Image(
                const RenderDevice *device,
                const crimild::Image *source
            ) noexcept;

            Image(
                const RenderDevice *device,
                const VkExtent2D &extent,
                VkFormat format,
                VkImageUsageFlags usage,
                std::string name = "Image"
            ) noexcept;

            /**
                \brief Constructs an image from an already allocated resource

                \remarks Sets m_readonly to true

                \remarks Internal use only
            */
            Image( const RenderDevice *rd, VkImage image, const VkExtent3D &extent, std::string name = "Image" ) noexcept;

            virtual ~Image( void ) noexcept;

            inline VkImage getHandle( void ) const noexcept { return m_handle; }

            inline VkExtent3D getExtent( void ) const noexcept { return m_extent; }
            inline VkFormat getFormat( void ) const noexcept { return m_format; }

            inline uint32_t getMipLevels( void ) const noexcept { return m_mipLevels; }
            inline uint32_t getArrayLayers( void ) const noexcept { return m_arrayLayers; }

            inline VkImageAspectFlags getAspectFlags( void ) const noexcept { return m_aspectFlags; }

            void allocateMemory( void ) noexcept;
            void allocateMemory( const VkMemoryAllocateInfo &allocateInfo ) noexcept;

            void transitionLayout( VkImageLayout newLayout ) const noexcept;
            void transitionLayout( VkImageLayout oldLayout, VkImageLayout newLayout ) const noexcept;
            void transitionLayout( VkCommandBuffer commandBuffer, VkImageLayout newLayout ) const noexcept;
            void transitionLayout( VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout ) const noexcept;

            void copy( VkCommandBuffer commandBuffer, SharedPointer< Image > const &src ) noexcept;
            void copy( VkCommandBuffer commandBuffer, SharedPointer< Image > const &src, const VkImageCopy &copyRegion ) noexcept;

            inline void setLayout( VkImageLayout layout ) noexcept { m_layout = layout; }
            inline VkImageLayout getLayout( void ) const noexcept { return m_layout; }

        private:
            VkImage m_handle = VK_NULL_HANDLE;
            VkDeviceMemory m_memory = VK_NULL_HANDLE;

            VkFormat m_format = VK_FORMAT_UNDEFINED;
            VkExtent3D m_extent = { 1, 1, 1 };
            uint32_t m_mipLevels = 1;
            uint32_t m_arrayLayers = 1;
            VkImageAspectFlags m_aspectFlags;

            mutable VkImageLayout m_layout = VK_IMAGE_LAYOUT_UNDEFINED;

            // Read-only images cannot be destroyed (i.e. swapchain images).
            bool m_readonly = false;
        };

    }

}

#endif
