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

#include "Editor/Panels/RenderScenePanel.hpp"

#include "Foundation/ImGUIUtils.hpp"
#include "Simulation/Simulation.hpp"

#include <sstream>

using namespace crimild;
using namespace crimild::editor;

RenderScenePanel::RenderScenePanel( vulkan::RenderDevice *renderDevice, const Point2 &position, const Extent2D &extent ) noexcept
    : editor::layout::Panel( "Render" ),
      m_pos( position ),
      m_extent( extent ),
      m_scenePass( renderDevice )
{
    // no-op
}

Event RenderScenePanel::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE:
            // Don't forward resize to scene pass since it will be handled
            // during update.
            break;

        default:
            m_scenePass.handle( e );
            break;
    }

    return Panel::handle( e );
}

void RenderScenePanel::render( void ) noexcept
{
    Panel::render();

    bool open = true;
    ImGui::Begin( getName().c_str(), &open, ImGuiWindowFlags_NoCollapse );
    ImVec2 actualSize = ImGui::GetContentRegionAvail();
    if ( actualSize.x != m_extent.width || actualSize.y != m_extent.height ) {
        m_extent.width = actualSize.x;
        m_extent.height = actualSize.y;
        m_lastResizeEvent = Event {
            .type = Event::Type::WINDOW_RESIZE,
            .extent = m_extent,
        };
    }

    if ( m_lastResizeEvent.type != Event::Type::NONE ) {
        m_scenePass.handle( m_lastResizeEvent );
        m_lastResizeEvent = Event {};
    }

    const auto att = m_scenePass.getColorAttachment();
    if ( !att->descriptorSets.empty() ) {
        ImTextureID tex_id = ( ImTextureID ) ( void * ) att->descriptorSets.data();
        ImVec2 uv_min = ImVec2( 0.0f, 0.0f );                 // Top-left
        ImVec2 uv_max = ImVec2( 1.0f, 1.0f );                 // Lower-right
        ImVec4 tint_col = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );   // No tint
        ImVec4 border_col = ImVec4( 1.0f, 1.0f, 1.0f, 0.0f ); // 50% opaque white
        ImGui::Image( tex_id, ImGui::GetContentRegionAvail(), uv_min, uv_max, tint_col, border_col );
    } else {
        ImGui::Text( "No scene attachments found" );
    }

    ImGui::End();

    if ( !open ) {
        removeFromParent();
        return;
    }

    m_scenePass.render( Simulation::getInstance()->getScene(), Simulation::getInstance()->getMainCamera() );
}
