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

#include "Editor/Panels/SimulationPanel.hpp"

#include "Editor/EditorLayer.hpp"
#include "Foundation/ImGUIUtils.hpp"
#include "Rendering/VulkanRenderDevice.hpp"

using namespace crimild;
using namespace crimild::editor;

bool SimulationPanel::s_visible = true;

SimulationPanel::SimulationPanel( vulkan::RenderDevice *renderDevice ) noexcept
    : RenderPass( renderDevice ),
      m_scenePass( renderDevice )
{
    // no-op
}

Event SimulationPanel::handle( const Event &e ) noexcept
{
    return m_scenePass.handle( e );
}

void SimulationPanel::render( void ) noexcept
{
    m_scenePass.render();

    const auto attachments = std::array< const vulkan::FramebufferAttachment *, 2 > {
        m_scenePass.getColorAttachment(),
        m_scenePass.getDepthAttachment()
    };

    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    for ( const auto att : attachments ) {
        getRenderDevice()->transitionImageLayout(
            commandBuffer,
            att->image,
            att->format,
            getRenderDevice()->formatIsColor( att->format ) ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            getRenderDevice()->formatIsColor( att->format ) ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            att->mipLevels,
            att->layerCount );
    }
}

void SimulationPanel::updateUI( EditorLayer *, bool ) noexcept
{
    if ( !isVibisle() ) {
        return;
    }

    const auto attachments = std::array< const vulkan::FramebufferAttachment *, 2 > {
        m_scenePass.getColorAttachment(),
        m_scenePass.getDepthAttachment()
    };

    static size_t selectedRenderMode = 0;
    if ( ImGui::BeginCombo( "Select render mode", attachments[ 0 ]->name.c_str(), 0 ) ) {
        for ( size_t i = 0; i < attachments.size(); ++i ) {
            if ( ImGui::Selectable( attachments[ i ]->name.c_str(), selectedRenderMode == i ) ) {
                selectedRenderMode = i;
            }
        }
        ImGui::EndCombo();
    }

    const auto att = attachments[ selectedRenderMode ];
    if ( !att->descriptorSets.empty() ) {
        ImTextureID tex_id = ( ImTextureID )( void * ) att->descriptorSets.data();
        ImVec2 uv_min = ImVec2( 0.0f, 0.0f );                 // Top-left
        ImVec2 uv_max = ImVec2( 1.0f, 1.0f );                 // Lower-right
        ImVec4 tint_col = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );   // No tint
        ImVec4 border_col = ImVec4( 1.0f, 1.0f, 1.0f, 0.0f ); // 50% opaque white
        ImGui::Image( tex_id, ImGui::GetContentRegionAvail(), uv_min, uv_max, tint_col, border_col );
    } else {
        ImGui::Text( "No scene attachments found" );
    }
}
