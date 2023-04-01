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

#ifndef CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_RENDER_SCENE
#define CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_RENDER_SCENE

#include "Rendering/FrameGraph/VulkanRenderBase.hpp"
#include "Rendering/VulkanSceneRenderState.hpp"
#include "Rendering/VulkanSynchronization.hpp"

namespace crimild {

    class Camera;
    class Node;

    namespace vulkan {

        class RenderTarget;

        namespace framegraph {

            class ComputeSceneLighting;
            class RenderSceneGBuffer;
            class RenderSceneLighting;
            class RenderSceneUnlit;
            class RenderShadowMaps;

            class RenderScene : public RenderBase {
            public:
                RenderScene(
                    RenderDevice *device,
                    std::string name,
                    std::shared_ptr< Camera > const &camera,
                    std::shared_ptr< ImageView > const &output,
                    SyncOptions const &options = {}
                ) noexcept;

                virtual ~RenderScene( void ) = default;

                virtual void execute( void ) noexcept override;

                inline const std::shared_ptr< RenderTarget > &getOutput( void ) const noexcept
                {
                    return getRenderTarget( getName() + "/Targets/Color" );
                }

                inline std::shared_ptr< RenderTarget > &getOutput( void ) noexcept
                {
                    return getRenderTarget( getName() + "/Targets/Color" );
                }

                inline const std::shared_ptr< RenderTarget > &getRenderTarget( std::string name ) const noexcept
                {
                    return m_renderTargets.at( name );
                }

                inline std::shared_ptr< RenderTarget > &getRenderTarget( std::string name ) noexcept
                {
                    return m_renderTargets.at( name );
                }

            protected:
                virtual void onResize( void ) noexcept override;

            private:
                std::shared_ptr< Camera > m_camera;
                std::shared_ptr< vulkan::ImageView > m_output;
                SyncOptions m_syncOptions;

                std::unordered_map< std::string, std::shared_ptr< RenderTarget > > m_renderTargets;

                std::shared_ptr< RenderShadowMaps > m_shadows;
                std::shared_ptr< RenderSceneGBuffer > m_gBuffer;
                std::shared_ptr< RenderSceneLighting > m_lighting;
                std::shared_ptr< RenderSceneUnlit > m_unlit;
                std::shared_ptr< RenderSceneUnlit > m_environment;

                std::shared_ptr< ComputeSceneLighting > m_compute;
            };

        }

    }

}

#endif
