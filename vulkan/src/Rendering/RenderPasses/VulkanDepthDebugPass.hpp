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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_PASSES_DEPTH_DEBUG_
#define CRIMILD_VULKAN_RENDERING_RENDER_PASSES_DEPTH_DEBUG_

#include "Mathematics/Matrix4_constants.hpp"
#include "Mathematics/Vector2.hpp"
#include "Rendering/RenderPasses/VulkanRenderPass.hpp"

namespace crimild {

    class UniformBuffer;

    namespace vulkan {

        class RenderDevice;
        class GraphicsPipeline;

        class DepthDebugPass : public RenderPass {
        public:
            DepthDebugPass( RenderDevice *renderDevice, std::string name, const FramebufferAttachment *depthInput ) noexcept;
            virtual ~DepthDebugPass( void ) noexcept;

            Event handle( const Event & ) noexcept;
            void render( void ) noexcept;

            inline void setNear( Real near ) { m_near = near; }
            inline Real getNear( void ) const { return m_near; }

            inline void setFar( Real far ) { m_far = far; }
            inline Real getFar( void ) const { return m_far; }

            [[nodiscard]] inline const FramebufferAttachment *getColorAttachment( void ) const noexcept { return &m_colorAttachment; }

        private:
            void init( void ) noexcept;
            void clear( void ) noexcept;

            void createRenderPassObjects( void ) noexcept;
            void destroyRenderPassObjects( void ) noexcept;

            void drawPrimitive( VkCommandBuffer cmds, Index currentFrameIndex, Primitive *primitive ) noexcept;

        private:
            std::string m_name;

            VkRenderPass m_renderPass = VK_NULL_HANDLE;
            std::vector< VkFramebuffer > m_framebuffers;
            VkRect2D m_renderArea;

            vulkan::FramebufferAttachment m_colorAttachment;

            Real m_near = Real( 1 );
            Real m_far = Real( 1000 );

            std::unique_ptr< GraphicsPipeline > m_pipeline;

            struct RenderPassObjects {
                VkDescriptorPool pool = VK_NULL_HANDLE;
                VkDescriptorSetLayout layout = VK_NULL_HANDLE;
                std::vector< VkDescriptorSet > descriptorSets;

                struct Uniforms {
                    alignas( 4 ) Real near;
                    alignas( 4 ) Real far;
                };

                std::unique_ptr< UniformBuffer > uniforms;
            } m_renderPassObjects;

            const vulkan::FramebufferAttachment *m_depthInput = nullptr;
        };

    }

}

#endif
