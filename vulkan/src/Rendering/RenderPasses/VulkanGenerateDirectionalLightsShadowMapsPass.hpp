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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_PASSES_GENERATE_DIRECTIONAL_LIGHTS_SHADOW_MAPS_
#define CRIMILD_VULKAN_RENDERING_RENDER_PASSES_GENERATE_DIRECTIONAL_LIGHTS_SHADOW_MAPS_

#include "Rendering/RenderPasses/VulkanRenderPassBase.hpp"
#include "Rendering/VulkanFramebufferAttachment.hpp"
#include "Rendering/VulkanSceneRenderState.hpp"
#include "Simulation/Event.hpp"

namespace crimild {

    class UniformBuffer;
    class Simulation;
    class Light;
    class Primitive;
    class Geometry;
    class Node;
    class Camera;

    namespace vulkan {

        class Framebuffer;
        class Image;
        class RenderDevice;
        class RenderPass;
        class GraphicsPipeline;

        /**
         * \brief Generate shadows for directional lights
         *
         * \details This pass generates shadow maps only for directional lights. For each directional light,
         * we generate its shadow map using a shared framebuffer which is later copied into the lights
         * own shadow map image.
         *
         * This pass implements cascade shadow mapping, splitting the camera frustum into multiple frustums,
         * each getting its own full-resolution shadow map. This is implemented using a multi-layered,
         * depth-only image.
         *
         * \todo Support different shadow resultions (HIGH, MED, LOW). Not sure if custom resulutions should
         * be supported, though.
         *
         * \todo Use geometry shaders to do a single-pass render for the depth map cascades.
         */
        class GenerateDirectionalLightsShadowMaps : public RenderPassBase {
        public:
            GenerateDirectionalLightsShadowMaps( RenderDevice *renderDevice ) noexcept;
            virtual ~GenerateDirectionalLightsShadowMaps( void ) noexcept;

            Event handle( const Event & ) noexcept;

            void render(
                const SceneRenderState::Lights &lights,
                const SceneRenderState::ShadowCasters &shadowCasters,
                const Camera *camera
            ) noexcept;

        private:
            void init( void ) noexcept;
            void clear( void ) noexcept;

            void renderShadowMapImage(
                const Light *light,
                const SceneRenderState::ShadowCasters &shadowCasters,
                const Matrix4f &lightSpaceMatrix,
                SharedPointer< vulkan::Image > const &shadowMapImage,
                uint32_t layerIndex
            ) noexcept;

            void createRenderPassObjects( void ) noexcept;
            void destroyRenderPassObjects( void ) noexcept;

            void createGeometryObjects( void ) noexcept;
            void bindGeometryDescriptors( VkCommandBuffer cmds, Index currentFrameIndex, Geometry *geometry ) noexcept;
            void destroyGeometryObjects( void ) noexcept;

            void drawPrimitive( VkCommandBuffer cmds, const Primitive *primitive ) noexcept;

        private:
            SharedPointer< vulkan::RenderPass > m_renderPass;
            std::vector< SharedPointer< vulkan::Framebuffer > > m_framebuffers;

            vulkan::FramebufferAttachment m_shadowAttachment;

            std::unique_ptr< GraphicsPipeline > m_pipeline;
        };

    }

}

#endif