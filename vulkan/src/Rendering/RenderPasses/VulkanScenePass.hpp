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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_PASSES_SCENE_
#define CRIMILD_VULKAN_RENDERING_RENDER_PASSES_SCENE_

#include "Foundation/VulkanUtils.hpp"
#include "Rendering/RenderPasses/VulkanRenderPass.hpp"

namespace crimild {

    class UniformBuffer;

    namespace vulkan {

        class GraphicsPipeline;

        class ScenePass : public RenderPass {
        public:
            explicit ScenePass( RenderDevice *renderDevice ) noexcept;
            virtual ~ScenePass( void ) noexcept;

            virtual void handle( const Event & ) noexcept override;
            virtual void render( void ) noexcept override;

        private:
            void init( void ) noexcept;
            void clear( void ) noexcept;

            void beginRenderPass( VkCommandBuffer commandBuffer, uint8_t currentFrameIndex ) noexcept;
            void endRenderPass( VkCommandBuffer commandBuffer ) noexcept;

            void createDescriptorPool( void ) noexcept;
            void destroyDescriptorPool( void ) noexcept;

            void createDescriptorSetLayout( void ) noexcept;
            void destroyDescriptorSetLayout( void ) noexcept;

            void createDescriptorSets( void ) noexcept;
            void destroyDescriptorSets( void ) noexcept;

        private:
            vulkan::RenderDevice *m_renderDevice = nullptr;
            VkRenderPass m_renderPass = VK_NULL_HANDLE;
            std::vector< VkFramebuffer > m_framebuffers;
            VkRect2D m_renderArea;

            std::unique_ptr< GraphicsPipeline > m_pipeline;

            std::unique_ptr< UniformBuffer > m_uniforms;

            VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
            VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
            std::vector< VkDescriptorSet > m_descriptorSets;
        };

    }

}

#endif