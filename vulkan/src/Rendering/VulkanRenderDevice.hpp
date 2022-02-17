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
#include "Rendering/VulkanShaderCompiler.hpp"

#include <vector>

namespace crimild {

    struct Event;

    namespace vulkan {

        class PhysicalDevice;
        class VulkanSurface;

        class RenderDevice {
        public:
            RenderDevice( PhysicalDevice *physicalDevice, VulkanSurface *surface, const Extent2D &extent ) noexcept;
            virtual ~RenderDevice( void ) noexcept;

            [[nodiscard]] inline const VkDevice &getHandle( void ) const noexcept { return m_handle; }

            [[nodiscard]] inline const PhysicalDevice *getPhysicalDevice( void ) const noexcept { return m_physicalDevice; }
            [[nodiscard]] inline const VulkanSurface *getSurface( void ) const noexcept { return m_surface; }

            [[nodiscard]] inline const VkExtent2D &getSwapchainExtent( void ) const noexcept { return m_swapchainExtent; }
            [[nodiscard]] inline const VkFormat &getSwapchainFormat( void ) const noexcept { return m_swapchainFormat; }
            [[nodiscard]] inline const std::vector< VkImageView > &getSwapchainImageViews( void ) const noexcept { return m_swapchainImageViews; }

            [[nodiscard]] inline uint8_t getCurrentFrameIndex( void ) noexcept { return m_imageIndex; }
            [[nodiscard]] inline VkCommandBuffer getCurrentCommandBuffer( void ) const noexcept { return m_commandBuffers[ m_imageIndex ]; }

            void handle( const Event &e ) noexcept;

            bool beginRender( void ) noexcept;
            bool endRender( void ) noexcept;

            void flush( void ) noexcept;

            inline ShaderCompiler &getShaderCompiler( void ) noexcept { return m_shaderCompiler; }

        private:
            void createSwapchain( void ) noexcept;
            void destroySwapchain( void ) noexcept;

            void createSyncObjects( void ) noexcept;
            void destroySyncObjects( void ) noexcept;

            void createCommandPool( VkCommandPool &commandPool ) noexcept;
            void destroyCommandPool( VkCommandPool &commandPool ) noexcept;

            void createCommandBuffers( void ) noexcept;
            void destroyCommandBuffers( void ) noexcept;
            void createCommandBuffer( VkCommandBuffer &commandBuffer ) noexcept;
            void destroyCommandBuffer( VkCommandBuffer &commandBuffer ) noexcept;

        private:
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
        };

    }

}

#endif
