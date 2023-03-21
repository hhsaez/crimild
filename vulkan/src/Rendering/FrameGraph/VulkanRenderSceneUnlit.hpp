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

#ifndef CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_RENDER_SCENE_UNLIT
#define CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_RENDER_SCENE_UNLIT

#include "Rendering/FrameGraph/VulkanRenderSceneBase.hpp"
#include "Rendering/VulkanSceneRenderState.hpp"

namespace crimild {

    class UniformBuffer;
    class UnlitMaterial;

    namespace vulkan {

        class DescriptorSet;
        class Framebuffer;
        class GraphicsPipeline;
        class RenderPass;
        class RenderTarget;

    }

}

namespace crimild::vulkan::framegraph {

    class RenderSceneUnlit
        : public RenderBase,
          public WithCommandBuffer {
    public:
        RenderSceneUnlit(
            RenderDevice *device,
            const VkExtent2D &extent,
            std::shared_ptr< RenderTarget > const &depthTarget,
            std::shared_ptr< RenderTarget > const &colorTarget
        ) noexcept;

        virtual ~RenderSceneUnlit( void ) = default;

        virtual void render(
            const SceneRenderState::RenderableSet< UnlitMaterial > &sceneRenderables,
            const Camera *camera
        ) noexcept;

    private:
        void bindMaterial( const UnlitMaterial *material ) noexcept;

    private:
        std::shared_ptr< RenderTarget > m_depthTarget;
        std::shared_ptr< RenderTarget > m_colorTarget;

        struct Resources {
            struct Common {
                std::shared_ptr< RenderPass > renderPass;
                std::shared_ptr< Framebuffer > framebuffer;

                struct UniformData {
                    Matrix4f view;
                    Matrix4f proj;
                };
                std::shared_ptr< UniformBuffer > uniforms;
                std::shared_ptr< DescriptorSet > descriptorSet;
            } common;

            struct MaterialResources {
                std::shared_ptr< DescriptorSet > descriptorSet;
                std::shared_ptr< GraphicsPipeline > pipeline;
                std::shared_ptr< UniformBuffer > uniforms;
            };
            std::unordered_map< const UnlitMaterial *, MaterialResources > materials;
        } m_resources;
    };

}

#endif
