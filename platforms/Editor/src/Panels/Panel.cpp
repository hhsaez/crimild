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

#include "Panels/Panel.hpp"

using namespace crimild::editor::panels;

Panel::Panel( void ) noexcept
{
    getAllPanels().push_back( this );
}

Panel::~Panel( void ) noexcept
{
    getAllPanels().remove( this );
}

crimild::Event Panel::handle( const crimild::Event &e ) noexcept
{
    return e;
}

void Panel::render( void ) noexcept
{
    // Keep track if the panel open state changes (see notes below)
    bool wasOpened = isOpen();

    if ( !m_loaded ) {
        // When the panel is first loaded, check ImGui settings to see if it is
        // actually opened. If not, Keep the panel closed.
        auto &context = *GImGui;
        for ( auto *settings = context.SettingsWindows.begin(); settings != nullptr; settings = context.SettingsWindows.next_chunk( settings ) ) {
            if ( strcmp( "SceneRT", settings->GetName() ) == 0 ) {
                std::cout << "test" << std::endl;
            }
            if ( strcmp( getTitle(), settings->GetName() ) == 0 ) {
                m_open = true;
            }
        }
        m_loaded = true;
    }

    int32_t flags = 0;
    if ( !isOpen() ) {
        // We still need to render the panel even if it is closed. So,
        // move it outside of the view and do not save it to ImGUI settings
        flags = ImGuiTableFlags_NoSavedSettings;
        ImGui::SetNextWindowPos( ImVec2( 999999, 999999 ) );
    } else if ( m_reopened ) {
        // Makes sure the panel is visible when re-opened
        auto pos = ImGui::GetMainViewport()->Pos;
        ImGui::SetNextWindowPos( pos + ImVec2( 100, 100 ) );
        ImGui::SetNextWindowSize( ImVec2( 300, 300 ) );
        m_reopened = false;
    }

    auto visible = ImGui::Begin( getTitle(), &m_open, flags );
    if ( m_open && visible ) {
        // If window is opened and visible, we should also check if it is actually
        // wihtin the viewport's boundaries (since it might be rendered beyond them)
        // in order to render its contents.
        const auto pos = ImGui::GetWindowPos();
        const auto visibleViewport = ImGui::GetMainViewport();
        const auto visibleViewportMin = visibleViewport->Pos;
        const auto visibleViewportMax = visibleViewportMin + visibleViewport->Size;
        const auto visibleX = pos.x >= visibleViewportMin.x && pos.x <= visibleViewportMax.x;
        const auto visibleY = pos.y >= visibleViewportMin.y && pos.y <= visibleViewportMax.y;
        if ( visibleX && visibleY ) {
            onRender();
        }
    }
    ImGui::End();

    if ( isOpen() != wasOpened ) {
        // Marks ImGui settings as dirty whenever the panel's open state changes
        // This forces ImGui settings to be saved to disk as soon as possible.
        ImGui::MarkIniSettingsDirty();
    }
}