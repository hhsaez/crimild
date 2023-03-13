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

#ifndef CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_RENDER_GBUFFER
#define CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_RENDER_GBUFFER

#include "Mathematics/Matrix4_constants.hpp"
#include "Rendering/FrameGraph/VulkanRenderBase.hpp"
#include "Rendering/VulkanSceneRenderState.hpp"

namespace crimild {

    class UniformBuffer;

    namespace materials {

        class PrincipledBSDF;

    }

    namespace vulkan {

        class CommandBuffer;
        class DescriptorSet;
        class Framebuffer;
        class GraphicsPipeline;
        class Image;
        class RenderPass;
        class RenderTarget;

        namespace framegraph {

            class RenderGBuffer : public RenderBase {
            public:
                RenderGBuffer(
                    RenderDevice *device,
                    const VkExtent2D &extent,
                    const std::vector< std::shared_ptr< RenderTarget > > &renderTargets
                ) noexcept;

                virtual ~RenderGBuffer( void ) noexcept;

                inline std::vector< std::shared_ptr< RenderTarget > > &getRenderTargets( void ) noexcept { return m_renderTargets; }

                void invalidates( std::vector< std::shared_ptr< RenderTarget > > const &renderTargets ) noexcept;
                void flushes( std::vector< std::shared_ptr< RenderTarget > > const &renderTargets ) noexcept;

                void render(
                    const SceneRenderState::RenderableSet< materials::PrincipledBSDF > &sceneRenderables,
                    const Camera *camera
                ) noexcept;

            protected:
                virtual void onResize( void ) noexcept override;

            private:
                void createRenderPassResources( void ) noexcept;
                void destroyRenderPassResources( void ) noexcept;

                void createMaterialResources( void ) noexcept;
                void bindMaterial( const materials::PrincipledBSDF *material ) noexcept;
                void destroyMaterialResources( void ) noexcept;

                std::shared_ptr< CommandBuffer > &getCommandBuffer( void ) noexcept { return m_commandBuffer; }

            private:
                std::vector< std::shared_ptr< RenderTarget > > m_renderTargets;

                std::unordered_set< std::shared_ptr< Image > > m_imagesToInvalidate;
                std::unordered_set< std::shared_ptr< Image > > m_imagesToFlush;

                struct Resources {
                    struct RenderPassResources {
                        std::shared_ptr< RenderPass > renderPass;
                        std::shared_ptr< Framebuffer > framebuffer;

                        struct UniformData {
                            alignas( 16 ) Matrix4 view = Matrix4::Constants::IDENTITY;
                            alignas( 16 ) Matrix4 proj = Matrix4::Constants::IDENTITY;
                        };
                        std::shared_ptr< UniformBuffer > uniforms;
                        std::shared_ptr< DescriptorSet > descriptorSet;
                    };

                    struct MaterialResources {
                        std::shared_ptr< DescriptorSet > descriptorSet;
                        std::shared_ptr< GraphicsPipeline > pipeline;
                        std::shared_ptr< UniformBuffer > uniforms;
                    };

                    RenderPassResources renderPass;
                    std::unordered_map< const materials::PrincipledBSDF *, MaterialResources > materials;
                } m_resources;

                std::shared_ptr< CommandBuffer > m_commandBuffer;
            };

        }

    }

}

#endif
