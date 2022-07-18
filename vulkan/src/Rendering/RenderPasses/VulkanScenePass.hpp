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

#ifndef CRIMILD_VULKAN_RENDERING_RENDER_PASSES_SCENE_
#define CRIMILD_VULKAN_RENDERING_RENDER_PASSES_SCENE_

#include "Rendering/RenderPasses/VulkanGBufferPass.hpp"
#include "Rendering/RenderPasses/VulkanLocalLightingPass.hpp"
#include "Rendering/RenderPasses/VulkanShadowPass.hpp"
#include "Rendering/RenderPasses/VulkanSkyboxPass.hpp"
#include "Rendering/RenderPasses/VulkanUnlitPass.hpp"

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

        class ScenePass {
        public:
            explicit ScenePass( RenderDevice *renderDevice ) noexcept;
            virtual ~ScenePass( void ) noexcept = default;

            Event handle( const Event & ) noexcept;
            void render( Node *scene, Camera *camera ) noexcept;

            inline const vulkan::RenderDevice *getRenderDevice( void ) const noexcept { return m_renderDevice; }

            [[nodiscard]] inline const FramebufferAttachment *getColorAttachment( void ) const noexcept { return m_localLightingPass.getColorAttachment(); }

        private:
            vulkan::RenderDevice *m_renderDevice = nullptr;
            ShadowPass m_shadowPass;
            GBufferPass m_gBufferPass;
            LocalLightingPass m_localLightingPass;
            UnlitPass m_unlitPass;
            SkyboxPass m_skyboxPass;
        };

    }

}

#endif
