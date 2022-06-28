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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_PASSES_SOFT_RT_
#define CRIMILD_VULKAN_RENDERING_RENDER_PASSES_SOFT_RT_

#include "Foundation/VulkanUtils.hpp"
#include "Mathematics/ColorRGB.hpp"
#include "Mathematics/ColorRGBA.hpp"
#include "Mathematics/Ray3.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/dot.hpp"
#include "Rendering/RenderPasses/VulkanRenderPassBase.hpp"
#include "Visitors/RTAcceleration.hpp"

namespace crimild {

    class UniformBuffer;
    class Simulation;
    class Material;
    class Geometry;
    class Primitive;
    class Node;
    class Camera;

    namespace vulkan {

        class RenderDevice;
        class GraphicsPipeline;

        /**
         * \brief Renders the scene using a software RT algorithm
         *
         * \todo This should be a compute pass. Or maybe not a render pass at all since
         * it only updates an image with the output from the software algorithm.
         */
        class SoftRTPass : public RenderPassBase {
        public:
            explicit SoftRTPass( RenderDevice *renderDevice ) noexcept;
            virtual ~SoftRTPass( void ) noexcept;

            Event handle( const Event & ) noexcept;
            void render( Node *scene, Camera *camera ) noexcept;

            [[nodiscard]] inline const FramebufferAttachment *getColorAttachment( void ) const noexcept { return &m_colorAttachment; }

        private:
            void init( void ) noexcept;
            void clear( void ) noexcept;

            void createRenderPassObjects( void ) noexcept;
            void destroyRenderPassObjects( void ) noexcept;

            void createImageResources( void ) noexcept;
            void destroyImageResources( void ) noexcept;

            void setup( Node *scene, Camera *camera ) noexcept;
            void initializeRays( void ) noexcept;

            // Maybe rename to SampleInfo?
            struct RayInfo {
                Ray3 ray;
                ColorRGB sampleColor;
                ColorRGB accumColor;
                Vector2 uv;
                Int32 bounces;
                Int32 samples;
            };

            struct IntersectionResult {
                Int32 materialId = -1;
                Real t = numbers::POSITIVE_INFINITY;
                Point3 point;
                Normal3 normal;
                Bool frontFace;

                inline void setFaceNormal( const Ray3 &R, const Normal3 &N ) noexcept
                {
                    frontFace = dot( direction( R ), N ) < 0;
                    normal = frontFace ? N : -N;
                }
            };

            Ray3 getRay( RayInfo &rayInfo ) noexcept;
            Bool intersectPrim( const Ray3 &R, const RTAcceleration::Result &scene, Int32 nodeId, Real minT, Real maxT, IntersectionResult &result ) noexcept;
            Bool intersectNR( const Ray3 &R, const RTAcceleration::Result &scene, IntersectionResult &result ) noexcept;
            bool scatter( const RTAcceleration::Result &scene, const Ray3 &R, const IntersectionResult &result, Ray3 &scattered, ColorRGB &attenuation, Bool &shouldBounce ) noexcept;
            void doSampleBounce( RayInfo &rayInfo, const RTAcceleration::Result &scene ) noexcept;
            void onSampleCompleted( RayInfo &rayInfo ) noexcept;
            void renderScene( Node *scene, Camera *camera ) noexcept;
            void updateImage( size_t index ) noexcept;

        private:
            VkRenderPass m_renderPass = VK_NULL_HANDLE;
            std::vector< VkFramebuffer > m_framebuffers;
            VkRect2D m_renderArea;

            vulkan::FramebufferAttachment m_colorAttachment;

            std::unique_ptr< GraphicsPipeline > m_pipeline;

            struct RenderPassObjects {
                VkDescriptorPool pool = VK_NULL_HANDLE;
                VkDescriptorSetLayout layout = VK_NULL_HANDLE;
                std::vector< VkDescriptorSet > descriptorSets;
            } m_renderPassObjects;

            struct ImageResources {
                VkBuffer stagingBuffer = VK_NULL_HANDLE;
                VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
                VkImage image = VK_NULL_HANDLE;
                VkDeviceMemory memory = VK_NULL_HANDLE;
                VkImageView imageView = VK_NULL_HANDLE;
                VkSampler sampler = VK_NULL_HANDLE;
            };
            // One image resource per frame
            std::vector< ImageResources > m_imageResources;

            VkFormat m_imageFormat;
            std::vector< ColorRGBA > m_imageData;

            ColorRGB m_backgroundColor = { 0.5, 0.7, 1.0 };

            std::vector< RayInfo > m_rays;

            SharedPointer< Node > m_scene;
            SharedPointer< Camera > m_camera;

            RTAcceleration::Result m_acceleratedScene;
        };

    }

}

#endif
