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

#ifndef CRIMILD_VULKAN_INITIALIZERS_
#define CRIMILD_VULKAN_INITIALIZERS_

#include <vulkan/vulkan.h>

namespace crimild {

    namespace vulkan {

        namespace initializers {

            [[deprecated]] inline constexpr VkMemoryAllocateInfo memoryAllocateInfo( void ) noexcept
            {
                return VkMemoryAllocateInfo {
                    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                };
            }

            [[deprecated]] inline constexpr VkImageCreateInfo imageCreateInfo( VkImageType imageType = VK_IMAGE_TYPE_2D ) noexcept
            {
                return VkImageCreateInfo {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                    .flags = 0,
                    .imageType = imageType,
                    .mipLevels = 1,
                    .arrayLayers = 1,
                    .samples = VK_SAMPLE_COUNT_1_BIT,
                    .tiling = VK_IMAGE_TILING_OPTIMAL,
                    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                };
            }

            [[deprecated]] inline constexpr VkImageCreateInfo imageCubeCreateInfo( void ) noexcept
            {
                auto createInfo = imageCreateInfo();
                createInfo.imageType = VK_IMAGE_TYPE_2D;
                createInfo.arrayLayers = 6;
                createInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
                return createInfo;
            }

            [[deprecated]] inline constexpr VkImageViewCreateInfo imageViewCreateInfo( VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D ) noexcept
            {
                return VkImageViewCreateInfo {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .viewType = viewType,
                    .subresourceRange = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = 1,
                        .baseArrayLayer = 0,
                        .layerCount = 1,
                    },
                };
            }

            [[deprecated]] inline constexpr VkImageCopy imageCopy( void ) noexcept
            {
                VkImageCopy copyRegion = {};
                copyRegion.srcSubresource.baseArrayLayer = 0;
                copyRegion.srcSubresource.mipLevel = 0;
                copyRegion.srcSubresource.layerCount = 1;
                copyRegion.srcOffset = { 0, 0, 0 };
                copyRegion.dstSubresource.baseArrayLayer = 0;
                copyRegion.dstSubresource.mipLevel = 0;
                copyRegion.dstSubresource.layerCount = 1;
                copyRegion.dstOffset = { 0, 0, 0 };
                return copyRegion;
            }

            [[deprecated]] inline constexpr VkRenderPassBeginInfo renderPassBeginInfo( void ) noexcept
            {
                return VkRenderPassBeginInfo {
                    .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                    .clearValueCount = 0,
                    .pClearValues = nullptr,
                };
            }

            [[deprecated]] inline constexpr VkFramebufferCreateInfo framebufferCreateInfo( void ) noexcept
            {
                return VkFramebufferCreateInfo {
                    .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                    .pNext = nullptr,
                    .layers = 1,
                };
            }

        }

    }

}

#endif
