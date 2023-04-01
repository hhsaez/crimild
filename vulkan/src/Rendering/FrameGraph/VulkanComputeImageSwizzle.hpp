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

#ifndef CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_COMPUTE_IMAGE_SWIZZLE
#define CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_COMPUTE_IMAGE_SWIZZLE

#include "Rendering/FrameGraph/VulkanComputeBase.hpp"
#include "Rendering/VulkanSynchronization.hpp"

namespace crimild::vulkan {

    class ComputePipeline;
    class DescriptorSet;
    class ImageView;

    namespace framegraph {

        class ComputeImageSwizzle
            : public ComputeBase,
              public WithCommandBuffer {
        public:
            enum class Selector {
                RGBA = 0,
                RGB = 1,
                RRR = 2,
                GGG = 3,
                BBB = 4,
                NORMAL = 6,
            };

        public:
            ComputeImageSwizzle(
                RenderDevice *device,
                std::string name,
                std::shared_ptr< vulkan::ImageView > const &input,
                Selector selector,
                std::shared_ptr< vulkan::ImageView > const &output,
                SyncOptions const &options = {}
            ) noexcept;

            virtual ~ComputeImageSwizzle( void ) = default;

            virtual void execute( void ) noexcept override;

        private:
            std::shared_ptr< ImageView > m_input;
            Selector m_selector;
            std::shared_ptr< ImageView > m_output;
            SyncOptions m_syncOptions;

            struct PushConstantsData {
                alignas( 4 ) uint32_t selector;
            };

            std::shared_ptr< ComputePipeline > m_pipeline;
            std::shared_ptr< DescriptorSet > m_descriptorSet;
        };

    }

}

#endif
