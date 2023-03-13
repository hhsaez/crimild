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

#ifndef CRIMILD_VULKAN_RENDERING_FRAMEBUFFER_
#define CRIMILD_VULKAN_RENDERING_FRAMEBUFFER_

#include "Foundation/Named.hpp"
#include "Foundation/SharedObject.hpp"
#include "Foundation/VulkanUtils.hpp"
#include "Rendering/VulkanWithRenderDeviceDEPRECATED.hpp"

namespace crimild {

    namespace vulkan {

        class ImageView;
        class RenderPass;
        class RenderTarget;

        /**
         * Keep track of bound image views so they don't get released before time
         */
        class Framebuffer
            : public SharedObject,
              public Named,
              public WithRenderDevice,
              public WithHandle< VkFramebuffer > {
        public:
            Framebuffer(
                RenderDevice *device,
                std::string name,
                const VkExtent2D &extent,
                std::shared_ptr< RenderPass > &renderPass,
                const std::vector< std::shared_ptr< RenderTarget > > &renderTargets
            ) noexcept;

            virtual ~Framebuffer( void ) noexcept;

            inline const VkExtent2D &getExtent( void ) const noexcept { return m_extent; }

        private:
            VkExtent2D m_extent;
            std::shared_ptr< RenderPass > m_renderPass;
            std::vector< std::shared_ptr< ImageView > > m_imageViews;
        };

        //////////////////////////
        // DEPRECATED FROM HERE //
        //////////////////////////

        class RenderPassDEPRECATED;
        struct FramebufferAttachment;

        class [[deprecated]] FramebufferDEPRECATED
            : public SharedObject,
              public WithConstRenderDevice {
        public:
            static std::vector< SharedPointer< FramebufferDEPRECATED > > createInFlightFramebuffers(
                const RenderDevice *rd,
                const SharedPointer< RenderPassDEPRECATED > &renderPass,
                const std::vector< const FramebufferAttachment * > &attachments
            ) noexcept;

        public:
            FramebufferDEPRECATED( const RenderDevice *rd, const VkFramebufferCreateInfo &createInfo ) noexcept;

            virtual ~FramebufferDEPRECATED( void ) noexcept;

            operator VkFramebuffer() const noexcept { return m_framebuffer; }

        private:
            VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
        };

    }

}

#endif
