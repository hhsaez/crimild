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

#include "Panels/SimulationPanel.hpp"

#include "Concurrency/debounce.hpp"
#include "Foundation/ImGuiUtils.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild::editor::panels;

Simulation::Simulation( crimild::vulkan::RenderDevice *renderDevice ) noexcept
    : m_scenePass( renderDevice )
{
}

void Simulation::onRender( void ) noexcept
{
    static auto debouncedResize = concurrency::debounce(
        [ this ]( Event e ) {
            m_scenePass.handle( e );
            m_descriptorSets.clear();
        },
        100
    );

    ImVec2 renderSize = ImGui::GetContentRegionAvail();
    bool isMinimized = renderSize.x < 1 || renderSize.y < 1;
    if ( !isMinimized ) {
        if ( m_extent.width != renderSize.x || m_extent.height != renderSize.y ) {
            m_extent.width = renderSize.x;
            m_extent.height = renderSize.y;
            debouncedResize(
                Event {
                    .type = Event::Type::WINDOW_RESIZE,
                    .extent = m_extent,
                }
            );
        }
    }

    auto currentFrameIdx = m_scenePass.getRenderDevice()->getCurrentFrameIndex();

    const auto att = m_scenePass.getColorAttachment();
    if ( !att->descriptorSets.empty() ) {
        if ( !m_descriptorSets.contains( att ) ) {
            auto ds = std::vector< VkDescriptorSet >( m_scenePass.getRenderDevice()->getInFlightFrameCount() );
            for ( int i = 0; i < ds.size(); ++i ) {
                ds[ i ] = ImGui_ImplVulkan_AddTexture(
                    att->sampler,
                    ( VkImageView ) *att->imageViews[ currentFrameIdx ].get(),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                );
            }
            m_descriptorSets[ att ] = ds;
        }
        auto tex_id = m_descriptorSets.at( att )[ currentFrameIdx ];
        ImVec2 uv_min = ImVec2( 0.0f, 0.0f );                 // Top-left
        ImVec2 uv_max = ImVec2( 1.0f, 1.0f );                 // Lower-right
        ImVec4 tint_col = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );   // No tint
        ImVec4 border_col = ImVec4( 1.0f, 1.0f, 1.0f, 0.0f ); // 50% opaque white
        ImGui::Image( tex_id, ImGui::GetContentRegionAvail(), uv_min, uv_max, tint_col, border_col );
    } else {
        ImGui::Text( "No scene attachments found" );
    }

    if ( isMinimized ) {
        return;
    }

    auto camera = crimild::Simulation::getInstance()->getMainCamera();
    if ( camera != nullptr ) {
        camera->setAspectRatio( m_extent.width / m_extent.height );
    }
    m_scenePass.render( crimild::Simulation::getInstance()->getScene(), camera );
}
