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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_PASSES_DEBUG_ATTACHMENT_
#define CRIMILD_VULKAN_RENDERING_RENDER_PASSES_DEBUG_ATTACHMENT_

#include "Rendering/RenderPasses/VulkanRenderPassBase.hpp"
#include "Rendering/VulkanFramebufferAttachment.hpp"
#include "Simulation/Event.hpp"

namespace crimild {

    class Node;
    class Camera;
    class UniformBuffer;

    namespace vulkan {

        class ClearPass;
        class GraphicsPipeline;
        class RenderDevice;
        class ShaderPass;

        class DebugAttachmentPass : public RenderPassBase {
        public:
            explicit DebugAttachmentPass(
                RenderDevice *renderDevice,
                std::string_view name,
                const FramebufferAttachment *inputAttachment,
                std::string_view mode = "rgb"
            ) noexcept;
            virtual ~DebugAttachmentPass( void ) noexcept;

            Event handle( const Event & ) noexcept;
            void render( Node *scene, Camera *camera ) noexcept;

            [[nodiscard]] inline const FramebufferAttachment *getColorAttachment( void ) const noexcept { return &m_colorAttachment; }

        private:
            void init( void ) noexcept;
            void deinit( void ) noexcept;

        private:
            VkRect2D m_renderArea;

            SharedPointer< ClearPass > m_clear;
            SharedPointer< ShaderPass > m_shader;

            const FramebufferAttachment *m_inputAttachment = nullptr;
            FramebufferAttachment m_colorAttachment;

            SharedPointer< UniformBuffer > m_uniforms;
        };

    }

}

#endif
