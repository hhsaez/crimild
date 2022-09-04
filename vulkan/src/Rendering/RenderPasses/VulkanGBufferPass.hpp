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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_PASSES_GBUFFER_
#define CRIMILD_VULKAN_RENDERING_RENDER_PASSES_GBUFFER_

#include "Mathematics/Matrix4_constants.hpp"
#include "Rendering/RenderPasses/VulkanRenderPassBase.hpp"
#include "Rendering/VulkanFramebufferAttachment.hpp"
#include "Rendering/VulkanSceneRenderState.hpp"
#include "Simulation/Event.hpp"

namespace crimild {

    class UniformBuffer;
    class Simulation;
    class Geometry;
    class Primitive;
    class Camera;
    class Node;

    namespace materials {

        class PrincipledBSDF;

    }

    namespace vulkan {

        class GraphicsPipeline;
        class RenderDevice;

        /**
         * \brief Fills a G-Buffer
         *
         * Geomtries are rendered using Vulkan's push constants to avoid creating new descriptor
         * sets for each renderable.
         *
         * G-Buffer Layout:
         * - Depth: F32
         * - Color #0: Albedo (F32G32B32)
         * - Color #1: World Position (F32G32B32)
         * - Color #2: World Normal (F32G32B32)
         * - Color #3: Metallic (F32), Roughness (G32), Ambient Occlusion (B32)
         *
         * \todo Add instancing support.
         */
        class GBufferPass : public RenderPassBase {
        public:
            explicit GBufferPass( RenderDevice *renderDevice, std::vector< const FramebufferAttachment * > attachments ) noexcept;
            virtual ~GBufferPass( void ) noexcept;

            Event handle( const Event & ) noexcept;
            void render( SceneRenderState::RenderableSet< materials::PrincipledBSDF > &sceneRenderables, Camera *camera ) noexcept;

        private:
            void init( void ) noexcept;
            void clear( void ) noexcept;

            void createRenderPassObjects( void ) noexcept;
            void destroyRenderPassObjects( void ) noexcept;

            void createMaterialObjects( void ) noexcept;
            void bind( materials::PrincipledBSDF *material ) noexcept;
            void destroyMaterialObjects( void ) noexcept;

            void drawPrimitive( VkCommandBuffer cmds, Index currentFrameIndex, Primitive *primitive ) noexcept;

        private:
            VkRenderPass m_renderPass = VK_NULL_HANDLE;
            std::vector< VkFramebuffer > m_framebuffers;
            VkRect2D m_renderArea;

            std::vector< const FramebufferAttachment * > m_attachments;

            struct RenderPassObjects {
                VkDescriptorPool pool = VK_NULL_HANDLE;
                VkDescriptorSetLayout layout = VK_NULL_HANDLE;
                std::vector< VkDescriptorSet > descriptorSets;

                struct Uniforms {
                    alignas( 16 ) Matrix4 view = Matrix4::Constants::IDENTITY;
                    alignas( 16 ) Matrix4 proj = Matrix4::Constants::IDENTITY;
                };
                std::unique_ptr< UniformBuffer > uniforms;
            } m_renderPassObjects;

            // TODO: I wonder if some of this cache should go to RenderDevice instead
            struct MaterialObjects {
                VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
                std::unordered_map< materials::PrincipledBSDF *, std::unique_ptr< GraphicsPipeline > > pipelines;
                std::unordered_map< materials::PrincipledBSDF *, VkDescriptorPool > descriptorPools;
                std::unordered_map< materials::PrincipledBSDF *, std::vector< VkDescriptorSet > > descriptorSets;
                std::unordered_map< materials::PrincipledBSDF *, std::unique_ptr< UniformBuffer > > uniforms;
            } m_materialObjects;
        };

    }

}

#endif
