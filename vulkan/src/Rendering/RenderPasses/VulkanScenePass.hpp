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

#include "Rendering/RenderPasses/VulkanRenderPassBase.hpp"
#include "Rendering/VulkanFramebufferAttachment.hpp"
#include "Rendering/VulkanSceneRenderState.hpp"
#include "Simulation/Event.hpp"

namespace crimild {

    class Node;
    class Camera;

    namespace vulkan {

        class ClearPass;
        class GBufferPass;
        class GraphicsPipeline;
        class LocalLightingPass;
        class ShadowPass;
        class UnlitPass;
        class RenderDevice;

        /**
         * \brief Renders a scene
         *
         * This is a complex render pass which uses multiple sub-passes to produce the final
         * image for a given scene, applying lighting, shadows, transparency and other effects.
         *
         * The output is a color attachment (in HDR format), as well as the G-Buffer used to
         * produce it.
         *
         * This render pass uses the following sub-passes:
         *
         * Sub-pass #0: G-Buffer
         * The first step is to produce a G-Buffer will all of the scene information required
         * for computing deferred lighting.
         *
         * Sub-pass #1: Lighting & Shadows
         * Once the G-Buffer has been computed, the next step is to compute lighting and shadows.
         * This is done per-light.
         *
         * Sub-pass #2: Forward unlit
         * Renders unlit and transparent geometries using forward rendering. Writes into depth
         * and color composition attachments.
         *
         * Sub-pass #3: Skybox (TODO)
         * Last step is to render the skybox and other environmental objects. It is performed
         * last so we can use the depth-attachment as an occlusion device, avoiding rendering
         * fragments where the sky is not actually visible.
         *
         * Once the render pass is executed, all attachments are transitioned to SHADER_READ
         * state so they can be used by other passes (i.e. ambient occlusion, post-processing,
         * etc.)
         */
        class ScenePass : public RenderPassBase {
        public:
            explicit ScenePass( RenderDevice *renderDevice ) noexcept;
            virtual ~ScenePass( void ) noexcept;

            Event handle( const Event & ) noexcept;
            void render( Node *scene, Camera *camera ) noexcept;

            [[nodiscard]] inline const FramebufferAttachment *getColorAttachment( void ) const noexcept
            {
                // Assumes the last attachment is the composition one
                return &m_attachments.back();
            }

            [[nodiscard]] inline const FramebufferAttachment *getAttachment( Index index ) const noexcept { return &m_attachments[ index ]; }

            template< typename Fn >
            void eachAttachment( Fn fn ) const noexcept
            {
                for ( const auto &att : m_attachments ) {
                    fn( &att );
                }
            }

        private:
            void init( void ) noexcept;
            void deinit( void ) noexcept;

        private:
            VkRect2D m_renderArea;

            SharedPointer< ClearPass > m_clear;

            SharedPointer< ShadowPass > m_shadowPass;

            SharedPointer< GBufferPass > m_gBuffer;
            SharedPointer< LocalLightingPass > m_lighting;
            SharedPointer< UnlitPass > m_unlit;

            // Storage for all attachments in the G-Buffer, including one for depth
            // and another one for final composition.
            std::array< FramebufferAttachment, 6 > m_attachments;

            // Keep render state alive for each frame that is sent to the GPU,
            // ensuring command buffers work with valid data for as long as they are
            // alive.
            std::vector< SceneRenderState > m_inUseAssets;
        };

    }

}

#endif
