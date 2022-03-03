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

#include "Editor/Menus/helpMenu.hpp"

#include "Foundation/Version.hpp"
#include "imgui.h"

using namespace crimild;

void helpAboutDialog( bool &open ) noexcept
{
    if ( !open ) {
        return;
    }

    ImGui::SetNextWindowPos( ImVec2( 200, 200 ), ImGuiCond_Always );
    ImGui::SetNextWindowSize( ImVec2( 350, 120 ), ImGuiCond_Always );

    if ( ImGui::Begin( "About", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize ) ) {
        Version version;
        auto versionStr = version.getDescription();
        ImGui::Text( "%s", versionStr.c_str() );
        ImGui::Text( "http://crimild.hhsaez.com" );
        ImGui::Text( "" );
        ImGui::Text( "Copyright (c) 2002 - present, H. Hernan Saez" );
        ImGui::Text( "All rights reserved." );

        ImGui::End();
    }
}

void crimild::editor::helpMenu( void ) noexcept
{
    static bool showHelpAboutDialog = false;

    if ( ImGui::BeginMenu( "Help" ) ) {
        if ( ImGui::MenuItem( "About..." ) ) {
            showHelpAboutDialog = true;
        }
        ImGui::EndMenu();
    }

    helpAboutDialog( showHelpAboutDialog );
}
