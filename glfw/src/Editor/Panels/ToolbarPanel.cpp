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

#include "Editor/Panels/ToolbarPanel.hpp"

#include "Editor/EditorLayer.hpp"
#include "Foundation/ImGUIUtils.hpp"

using namespace crimild;

void editor::ToolbarPanel::render( void ) noexcept
{
    auto editor = EditorLayer::getInstance();
    auto simState = editor->getSimulationState();

    {
        ImGui::SetNextWindowPos( ImVec2( 0, 15 ), ImGuiCond_Always );
        ImGui::SetNextWindowSize( ImVec2( 1920, 10 ), ImGuiCond_Always );
        bool open = true;

        if ( ImGui::Begin( "Top Bar", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove ) ) {
            ImGui::SetCursorPosX( ImGui::GetCursorPosX() + 800 );

            if ( simState != SimulationState::PLAYING ) {
                if ( ImGui::Button( "Play" ) ) {
                    editor->setSimulationState( SimulationState::PLAYING );
                }
            } else {
                ImGui::BeginDisabled( true );
                ImGui::Button( "Play" );
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            if ( simState == SimulationState::PLAYING ) {
                if ( ImGui::Button( "Pause" ) ) {
                    editor->setSimulationState( SimulationState::PAUSED );
                }
            } else {
                ImGui::BeginDisabled( true );
                ImGui::Button( "Pause" );
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            if ( simState != SimulationState::STOPPED ) {
                if ( ImGui::Button( "Stop" ) ) {
                    editor->setSimulationState( SimulationState::STOPPED );
                }
            } else {
                ImGui::BeginDisabled( true );
                ImGui::Button( "Stop" );
                ImGui::EndDisabled();
            }

            ImGui::End();
        }
    }

    Layer::render();
}
