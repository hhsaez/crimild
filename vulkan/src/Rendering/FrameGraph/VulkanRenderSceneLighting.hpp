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

#ifndef CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_RENDER_SCENE_LIGHTING
#define CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_RENDER_SCENE_LIGHTING

#include "Rendering/FrameGraph/VulkanRenderSceneBase.hpp"

namespace crimild {

    class Primitive;
    class UniformBuffer;

    namespace vulkan {

        class CommandBuffer;
        class DescriptorSet;
        class DescriptorSetLayout;
        class Framebuffer;
        class GraphicsPipeline;
        class RenderPass;
        class RenderTarget;

    }

}

namespace crimild::vulkan::framegraph {

    class RenderSceneLighting
        : public RenderSceneBase,
          public WithCommandBuffer {
    public:
        RenderSceneLighting(
            RenderDevice *device,
            const VkExtent2D &extent,
            const std::vector< std::shared_ptr< RenderTarget > > &inputs,
            std::shared_ptr< RenderTarget > &output
        ) noexcept;

        virtual ~RenderSceneLighting( void ) = default;

        virtual void render( const SceneRenderState &renderState, const Camera *camera ) noexcept override;

    private:
        std::shared_ptr< DescriptorSet > getDirectionalLightDescriptors( const Light *light ) noexcept;
        std::shared_ptr< DescriptorSet > getPointLightDescriptors( const Light *light ) noexcept;
        std::shared_ptr< DescriptorSet > getSpotLightDescriptors( const Light *light ) noexcept;

    private:
        std::vector< std::shared_ptr< RenderTarget > > m_inputs;
        std::shared_ptr< RenderTarget > m_output;

        struct Resources {
            struct Common {
                std::shared_ptr< RenderPass > renderPass;
                std::shared_ptr< Framebuffer > framebuffer;

                struct UniformData {
                    Matrix4f view;
                    Matrix4f proj;
                    Vector2f viewport;
                };
                std::shared_ptr< UniformBuffer > uniforms;

                std::shared_ptr< DescriptorSet > descriptorSet;
            } common;

            struct Lights {
                std::shared_ptr< DescriptorSetLayout > descriptorSetLayout;

                struct Directional {
                    std::shared_ptr< GraphicsPipeline > pipeline;

                    struct UniformData {
                        alignas( 4 ) Real32 castShadows = 0.0f;
                        alignas( 16 ) Vector3f direction;
                        alignas( 16 ) ColorRGBf color;
                        alignas( 16 ) Matrix4f lightSpaceMatrices[ 4 ];
                        alignas( 16 ) Vector4f splits;
                    };

                    std::unordered_map< const Light *, std::shared_ptr< UniformBuffer > > uniforms;
                    std::unordered_map< const Light *, std::shared_ptr< DescriptorSet > > descriptorSets;
                } directional;

                struct Point {
                    std::shared_ptr< GraphicsPipeline > pipeline;
                    std::shared_ptr< Primitive > volume;

                    struct UniformData {
                        alignas( 16 ) Point3f position;
                        alignas( 4 ) Real32 radius;
                        alignas( 16 ) ColorRGBf color;
                        alignas( 4 ) float castShadows;
                    };

                    std::unordered_map< const Light *, std::shared_ptr< UniformBuffer > > uniforms;
                    std::unordered_map< const Light *, std::shared_ptr< DescriptorSet > > descriptorSets;
                } point;

                struct Spot {
                    std::shared_ptr< GraphicsPipeline > pipeline;
                    std::shared_ptr< Primitive > volume;

                    struct UniformData {
                        alignas( 4 ) Real32 castShadows = 0.0f;
                        alignas( 16 ) Point3f position;
                        alignas( 16 ) Vector3f direction;
                        alignas( 4 ) Real32 radius;
                        alignas( 16 ) ColorRGBf color;
                        alignas( 4 ) Real32 innerCutoff;
                        alignas( 4 ) Real32 outerCutoff;
                        alignas( 16 ) Matrix4f lightSpaceProjection;
                    };

                    std::unordered_map< const Light *, std::shared_ptr< UniformBuffer > > uniforms;
                    std::unordered_map< const Light *, std::shared_ptr< DescriptorSet > > descriptorSets;
                } spot;
            } lights;
        } m_resources;
    };

}

#endif
