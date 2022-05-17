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

#ifndef CRIMILD_GLFW_LAYER_IMGUI_
#define CRIMILD_GLFW_LAYER_IMGUI_

#include "Foundation/VulkanUtils.hpp"
#include "Mathematics/Clock.hpp"
#include "Mathematics/Vector4_constants.hpp"
#include "Rendering/Layer.hpp"

namespace crimild {

    namespace vulkan {

        class RenderDevice;
        class GraphicsPipeline;

    }

    class ImGUILayer : public Layer {
    public:
        explicit ImGUILayer( vulkan::RenderDevice *renderDevice ) noexcept;
        virtual ~ImGUILayer( void );

        virtual Event handle( const Event & ) noexcept override;
        virtual void render( void ) noexcept override;

    private:
        void updateDisplaySize( void ) const noexcept;

        void init( void ) noexcept;
        void clean( void ) noexcept;

        void createRenderPassObjects( void ) noexcept;
        void destroyRenderPassObjects( void ) noexcept;

        void createFontAtlas( void ) noexcept;
        void destroyFontAtlas( void ) noexcept;

    private:
        vulkan::RenderDevice *m_renderDevice = nullptr;
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        std::vector< VkFramebuffer > m_framebuffers;
        VkRect2D m_renderArea;

        std::unique_ptr< vulkan::GraphicsPipeline > m_pipeline;

        std::unique_ptr< ShaderProgram > m_program;

        struct RenderPassObjects {
            VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
            VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
            std::vector< VkDescriptorSet > descriptorSets;

            struct Uniforms {
                Vector4 scale = Vector4::Constants::ONE;
                Vector4 translate = Vector4::Constants::ZERO;
            };

            std::unique_ptr< UniformBuffer > uniforms;
        } m_renderPassObjects;

        std::unique_ptr< VertexBuffer > m_vertices;
        std::unique_ptr< IndexBuffer > m_indices;

        struct FontAtlas {
            VkImage image = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;
            VkImageView imageView = VK_NULL_HANDLE;
            VkSampler sampler = VK_NULL_HANDLE;
            VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
            VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
            std::vector< VkDescriptorSet > descriptorSets;
        } m_fontAtlas;

        Clock m_clock;
    };

}

#endif
