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

#include "Foundation/SharedObject.hpp"
#include "Foundation/VulkanUtils.hpp"
#include "Rendering/VulkanWithRenderDevice.hpp"

namespace crimild {

    namespace vulkan {

        class Image
            : public SharedObject,
              public WithConstRenderDevice {
        public:
            Image( const RenderDevice *rd, const VkImageCreateInfo &createInfo ) noexcept;

            /**
                \brief Constructs an image from an already allocated resource

                \remarks Sets m_readonly to true

                \remarks Internal use only
            */
            Image( const RenderDevice *rd, VkImage image, const VkExtent3D &extent ) noexcept;

            virtual ~Image( void ) noexcept;

            operator VkImage() const noexcept { return m_image; }

            inline VkExtent3D getExtent( void ) const noexcept { return m_extent; }
            inline VkFormat getFormat( void ) const noexcept { return m_format; }

            void setName( std::string_view name ) noexcept;

            void allocateMemory( void ) noexcept;
            void allocateMemory( const VkMemoryAllocateInfo &allocateInfo ) noexcept;

            void transitionLayout( VkImageLayout newLayout ) const noexcept;
            void transitionLayout( VkImageLayout oldLayout, VkImageLayout newLayout ) const noexcept;
            void transitionLayout( VkCommandBuffer commandBuffer, VkImageLayout newLayout ) const noexcept;
            void transitionLayout( VkCommandBuffer commandBuffer, VkImageLayout oldLayout, VkImageLayout newLayout ) const noexcept;

            void copy( VkCommandBuffer commandBuffer, SharedPointer< Image > const &src ) noexcept;
            void copy( VkCommandBuffer commandBuffer, SharedPointer< Image > const &src, const VkImageCopy &copyRegion ) noexcept;

        private:
            VkImage m_image = VK_NULL_HANDLE;
            VkDeviceMemory m_memory = VK_NULL_HANDLE;

            VkFormat m_format = VK_FORMAT_UNDEFINED;
            VkExtent3D m_extent = { 1, 1, 1 };
            uint32_t m_mipLevels = 1;
            uint32_t m_arrayLayers = 1;

            mutable VkImageLayout m_layout = VK_IMAGE_LAYOUT_UNDEFINED;

            // Read-only images cannot be destroyed (i.e. swapchain images).
            bool m_readonly = false;
        };

    }

}

#endif
