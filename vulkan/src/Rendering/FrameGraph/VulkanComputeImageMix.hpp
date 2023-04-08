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

#ifndef CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_COMPUTE_IMAGE_MIX
#define CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_COMPUTE_IMAGE_MIX

#include "Rendering/FrameGraph/VulkanComputeBase.hpp"
#include "Rendering/VulkanSynchronization.hpp"

namespace crimild::vulkan {

    class ComputePipeline;
    class DescriptorSet;
    class ImageView;

    namespace framegraph {

        class ComputeImageMix
            : public ComputeBase,
              public WithCommandBuffer {
        public:
            enum class Mode : uint32_t {
                ADD = 1,
                AVERAGE = 2,
                COLOR_BURN = 3,
                COLOR_DODGE = 4,
                DARKEN = 5,
                DIFFERENCE = 6,
                EXCLUSION = 7,
                GLOW = 8,
                HARD_LIGHT = 9,
                HARD_MIX = 10,
                LIGHTEN = 11,
                LINEAR_BURN = 12,
                LINEAR_DODGE = 13,
                LINEAR_LIGHT = 14,
                MULTIPLY = 15,
                NEGATION = 16,
                NORMAL = 17,
                OVERLAY = 18,
                PHOENIX = 19,
                PIN_LIGHT = 20,
                REFLECT = 21,
                SCREEN = 22,
                SOFT_LIGHT = 23,
                SUBTRACT = 24,
                VIVID_LIGHT = 25
            };

        public:
            ComputeImageMix(
                RenderDevice *device,
                std::string name,
                std::shared_ptr< ImageView > const &source,
                std::shared_ptr< ImageView > const &destination,
                std::shared_ptr< ImageView > const &output,
                Mode mode = Mode::NORMAL,
                SyncOptions const &options = {}
            ) noexcept;

            virtual ~ComputeImageMix( void ) = default;

            virtual void execute( void ) noexcept override;

        private:
            std::shared_ptr< ImageView > m_source;
            std::shared_ptr< ImageView > m_destination;
            std::shared_ptr< ImageView > m_output;
            SyncOptions m_syncOptions;

            std::shared_ptr< ComputePipeline > m_pipeline;
            std::shared_ptr< DescriptorSet > m_descriptorSet;
        };

    }

}

#endif
