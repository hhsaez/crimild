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
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_PASSES_RENDER_
#define CRIMILD_VULKAN_RENDERING_RENDER_PASSES_RENDER_

#include "Foundation/VulkanUtils.hpp"
#include "Simulation/Event.hpp"

namespace crimild {

    struct Event;

    namespace vulkan {

        class RenderDevice;

        struct FramebufferAttachment {
            std::string name;
            VkExtent2D extent;
            VkFormat format = VK_FORMAT_UNDEFINED;
            VkImage image = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;
            VkImageView imageView = VK_NULL_HANDLE;
            VkSampler sampler = VK_NULL_HANDLE;
            uint32_t mipLevels = 1;
            uint32_t layerCount = 1;
            VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE; // TODO: overkill?
            VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
            std::vector< VkDescriptorSet > descriptorSets;
        };

        class RenderPassBase {
        protected:
            explicit RenderPassBase( RenderDevice *renderDevice ) noexcept;

        public:
            virtual ~RenderPassBase( void ) = default;

            inline RenderDevice *getRenderDevice( void ) noexcept { return m_renderDevice; }
            inline const RenderDevice *getRenderDevice( void ) const noexcept { return m_renderDevice; }

        protected:
            void createFramebufferAttachment(
                std::string name,
                const VkExtent2D &extent,
                VkFormat format,
                FramebufferAttachment &out ) const;
            void destroyFramebufferAttachment( FramebufferAttachment &att ) const;

        private:
            RenderDevice *m_renderDevice = nullptr;
        };

    }

}

#endif