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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_PASS_
#define CRIMILD_VULKAN_RENDERING_RENDER_PASS_

#include "Foundation/VulkanUtils.hpp"
#include "Rendering/VulkanWithRenderDeviceDEPRECATED.hpp"

namespace crimild::vulkan {

    class RenderTarget;

    class RenderPass
        : public SharedObject,
          public Named,
          public WithRenderDevice,
          public WithHandle< VkRenderPass > {
    public:
        RenderPass(
            RenderDevice *device,
            std::string name,
            std::vector< std::shared_ptr< RenderTarget > > &renderTargets,
            VkAttachmentLoadOp loadOp
        ) noexcept;

        RenderPass(
            RenderDevice *device,
            std::string name,
            const VkRenderPassCreateInfo &createInfo
        ) noexcept;

        virtual ~RenderPass( void ) noexcept;

        inline const std::vector< VkClearValue > &getClearValues( void ) const noexcept { return m_clearValues; }

    private:
        std::vector< VkClearValue > m_clearValues;
    };

    class FramebufferDEPRECATED;
    struct FramebufferAttachment;

    class [[deprecated]] RenderPassDEPRECATED
        : public SharedObject,
          public WithConstRenderDevice {
    public:
        RenderPassDEPRECATED(
            RenderDevice *device,
            std::string name,
            std::vector< std::shared_ptr< RenderTarget > > &renderTargets,
            VkAttachmentLoadOp loadOp
        ) noexcept;

        RenderPassDEPRECATED(
            const RenderDevice *rd,
            const std::vector< const FramebufferAttachment * > &attachments,
            bool clearAttachments = false
        ) noexcept;

        RenderPassDEPRECATED(
            const vulkan::RenderDevice *rd,
            const VkRenderPassCreateInfo &createInfo
        ) noexcept;

        virtual ~RenderPassDEPRECATED( void ) noexcept;

        operator VkRenderPass() const noexcept { return m_renderPass; }

        void setName( std::string_view name ) const noexcept;

        void begin( VkCommandBuffer commandBuffer, const SharedPointer< vulkan::FramebufferDEPRECATED > &framebuffer ) const noexcept;
        void begin( VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo &beginInfo ) const noexcept;
        void end( VkCommandBuffer commandBuffer ) const noexcept;

    private:
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        std::vector< VkClearValue > m_clearValues;
        VkRect2D m_renderArea;
    };

}

#endif
