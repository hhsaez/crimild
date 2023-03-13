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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_PASSES_GENERATE_POINT_LIGHTS_SHADOW_MAPS_
#define CRIMILD_VULKAN_RENDERING_RENDER_PASSES_GENERATE_POINT_LIGHTS_SHADOW_MAPS_

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

        class FramebufferDEPRECATED;
        class RenderDevice;
        class RenderPassDEPRECATED;
        class GraphicsPipeline;

        /**
         * \brief Generate shadows for point lights
         *
         * \details This pass generates shadow maps only for point lights. For each point light,
         * we generate its shadow map using a shared framebuffer which is later copied into the lights
         * own shadow map image.
         *
         * \todo Support different shadow resultions (HIGH, MED, LOW). Not sure if custom resulutions should
         * be supported, though.
         *
         * \todo Generate cascaded shadow maps instead of simple ones.
         */
        class GeneratePointLightsShadowMaps : public RenderPassBase {
        public:
            GeneratePointLightsShadowMaps( RenderDevice *renderDevice ) noexcept;
            virtual ~GeneratePointLightsShadowMaps( void ) noexcept;

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
                SharedPointer< vulkan::Image > &shadowMapImage,
                uint32_t layerIndex
            ) noexcept;

            void createLightObjects( void ) noexcept;
            void bindLight( const Light *light ) noexcept;
            void destroyLightObjects( void ) noexcept;

            void drawPrimitive( VkCommandBuffer cmds, const Primitive *primitive ) noexcept;

        private:
            std::shared_ptr< RenderPassDEPRECATED > m_renderPass;
            std::vector< std::shared_ptr< FramebufferDEPRECATED > > m_framebuffers;

            vulkan::FramebufferAttachment m_colorAttachment;
            vulkan::FramebufferAttachment m_depthAttachment;

            // Define a set of objects that are set per-layer of the cubemap
            // One set of objects per light.
            // TODO: This seems inefficient...
            struct LightObjects {
                VkDescriptorSetLayout layout = VK_NULL_HANDLE;

                struct PerLayer {
                    VkDescriptorPool pool = VK_NULL_HANDLE;
                    std::vector< VkDescriptorSet > descriptorSets;

                    struct Uniforms {
                        alignas( 16 ) Matrix4f lightSpaceMatrix = Matrix4::Constants::IDENTITY;
                        alignas( 16 ) Vector3f lightPosition;
                    };

                    std::unique_ptr< UniformBuffer > uniforms;
                };
                std::unordered_map< const Light *, std::array< PerLayer, 6 > > lights;
            } m_lightObjects;

            std::unique_ptr< GraphicsPipeline > m_pipeline;

            struct PerGeometryUniforms {
                alignas( 16 ) Matrix4f model;
            };
        };

    }

}

#endif
