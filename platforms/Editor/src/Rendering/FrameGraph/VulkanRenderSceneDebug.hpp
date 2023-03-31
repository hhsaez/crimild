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

#ifndef CRIMILD_EDITOR_RENDERING_FRAME_GRAPH_RENDER_SCENE_DEBUG
#define CRIMILD_EDITOR_RENDERING_FRAME_GRAPH_RENDER_SCENE_DEBUG

#include "Mathematics/Matrix4.hpp"
#include "Rendering/FrameGraph/VulkanRenderBase.hpp"
#include "Rendering/VulkanSynchronization.hpp"

namespace crimild {

    class Camera;
    class Node;
    class UniformBuffer;

    namespace vulkan {

        class DescriptorSet;
        class Framebuffer;
        class GraphicsPipeline;
        class RenderPass;
        class RenderTarget;

    }

}

namespace crimild::vulkan::framegraph {

    class RenderSceneDebug
        : public RenderBase,
          public WithCommandBuffer {
    public:
        RenderSceneDebug( RenderDevice *device, std::string name, const VkExtent2D &extent ) noexcept;
        virtual ~RenderSceneDebug( void ) = default;

        inline std::shared_ptr< RenderTarget > &getOutput( void ) noexcept { return m_outputTarget; }

        virtual void render(
            Node *scene,
            Camera *camera,
            SyncOptions const &options = {}
        ) noexcept;

    private:
        std::shared_ptr< RenderTarget > m_outputTarget;

        struct PushConstantsData {
            Matrix4f mvp;
            ColorRGB color;
        };

        struct Resources {
            std::shared_ptr< RenderPass > renderPass;
            std::shared_ptr< Framebuffer > framebuffer;
            std::shared_ptr< GraphicsPipeline > pipeline;
        } m_resources;
    };

}

#endif
