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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_PASSES_GENERATE_SPOT_LIGHTS_SHADOW_MAPS_
#define CRIMILD_VULKAN_RENDERING_RENDER_PASSES_GENERATE_SPOT_LIGHTS_SHADOW_MAPS_

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
        class RenderDevice;
        class RenderPass;
        class GraphicsPipeline;

        /**
         * \brief Generate shadows for spot lights
         *
         * \details This pass generates shadow maps only for spot lights. For each spot light,
         * we generate its shadow map using a shared framebuffer which is later copied into the lights
         * own shadow map image.
         *
         * \todo Support different shadow resultions (HIGH, MED, LOW). Not sure if custom resulutions should
         * be supported, though.
         *
         * \todo Generate cascaded shadow maps instead of simple ones.
         */
        class GenerateSpotLightsShadowMaps : public RenderPassBase {
        public:
            GenerateSpotLightsShadowMaps( RenderDevice *renderDevice ) noexcept;
            virtual ~GenerateSpotLightsShadowMaps( void ) noexcept;

            Event handle( const Event & ) noexcept;

            void render(
                const SceneRenderState::Lights &lights,
                const SceneRenderState::ShadowCasters &shadowCasters,
                const Camera *camera
            ) noexcept;

        private:
            void init( void ) noexcept;
            void clear( void ) noexcept;

            [[nodiscard]] inline const FramebufferAttachment *getShadowAttachment( void ) const noexcept { return &m_shadowAttachment; }

            void renderShadowMap(
                const Light *light,
                const SceneRenderState::ShadowCasters &shadowCasters,
                const Matrix4f &lightSpaceMatrix,
                SharedPointer< vulkan::Image > const &shadowMapImage
            ) noexcept;

            void createRenderPassObjects( void ) noexcept;
            void destroyRenderPassObjects( void ) noexcept;

            void createGeometryObjects( void ) noexcept;
            void bindGeometryDescriptors( VkCommandBuffer cmds, Index currentFrameIndex, Geometry *geometry ) noexcept;
            void destroyGeometryObjects( void ) noexcept;

            void drawPrimitive( VkCommandBuffer cmds, const Primitive *primitive ) noexcept;

        private:
            std::shared_ptr< vulkan::RenderPass > m_renderPass;
            std::vector< std::shared_ptr< vulkan::Framebuffer > > m_framebuffers;

            vulkan::FramebufferAttachment m_shadowAttachment;

            std::unique_ptr< GraphicsPipeline > m_pipeline;
        };

    }

}

#endif
