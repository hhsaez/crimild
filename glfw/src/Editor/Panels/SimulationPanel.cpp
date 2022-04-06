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

using namespace crimild;
using namespace crimild::editor;

bool SimulationPanel::s_visible = true;

SimulationPanel::SimulationPanel( vulkan::RenderDevice *renderDevice ) noexcept
    : RenderPass( renderDevice ),
      m_gBufferPass( renderDevice )
{
    // no-op
}

Event SimulationPanel::handle( const Event &e ) noexcept
{
    return e;
}

void SimulationPanel::render( void ) noexcept
{
    // m_gBufferPass.render();
}

void SimulationPanel::updateUI( EditorLayer *, bool embedded ) noexcept
{
    if ( !isVibisle() ) {
        return;
    }

    if ( !embedded ) {
        ImGui::SetNextWindowPos( ImVec2( m_pos.x, m_pos.y ), ImGuiCond_Always );
        ImGui::SetNextWindowSize( ImVec2( m_extent.width, m_extent.height ), ImGuiCond_Always );

        if ( !ImGui::Begin( "Simulation", &s_visible ) ) {
            return;
        }
    }

    // ImTextureID tex_id = ( ImTextureID )( intptr_t )( 0 );
    // ImVec2 uv_min = ImVec2( 0.0f, 0.0f );                 // Top-left
    // ImVec2 uv_max = ImVec2( 1.0f, 1.0f );                 // Lower-right
    // ImVec4 tint_col = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );   // No tint
    // ImVec4 border_col = ImVec4( 1.0f, 1.0f, 1.0f, 0.0f ); // 50% opaque white
    // ImGui::Image( tex_id, ImGui::GetContentRegionAvail(), uv_min, uv_max, tint_col, border_col );

    if ( !embedded ) {
        ImGui::End();
    }
}
