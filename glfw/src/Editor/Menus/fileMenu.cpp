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

#include "Editor/Menus/fileMenu.hpp"

#include "Coding/JSONEncoder.hpp"
#include "Editor/EditorUtils.hpp"
#include "Foundation/ImGUIUtils.hpp"
#include "Foundation/Version.hpp"

#include <iostream>

void crimild::editor::fileMenu( void ) noexcept
{
    if ( ImGui::BeginMenu( "File" ) ) {
        if ( ImGui::MenuItem( "New Scene" ) ) {
            crimild::editor::loadNewScene();
        }

        if ( ImGui::MenuItem( "Open Scene..." ) ) {
            ImGuiFileDialog::Instance()->OpenDialog( "LoadSceneDlgKey", "Open Scene...", ".json", "." );
        }

        if ( ImGui::BeginMenu( "Open Recent" ) ) {
            if ( ImGui::MenuItem( "TODO" ) ) {
                // TODO
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if ( ImGui::MenuItem( "Save Scene" ) ) {
            // TODO
        }
        if ( ImGui::MenuItem( "Save Scene As..." ) ) {
            ImGuiFileDialog::Instance()->OpenDialog( "SaveSceneDlgKey", "Save Scene As...", ".json", "." );
        }

        ImGui::Separator();

        if ( ImGui::MenuItem( "Export..." ) ) {
            ImGuiFileDialog::Instance()->OpenDialog( "ExportSceneDlgKey", "Export scene", ".crimild", "." );
        }

        if ( ImGui::MenuItem( "Import..." ) ) {
            ImGuiFileDialog::Instance()->OpenDialog( "ChooseFileDlgKey", "Choose File", ".crimild,.obj,.gltf", "." );
        }

        ImGui::Separator();

        if ( ImGui::MenuItem( "Quit" ) ) {
            // crimild::concurrency::sync_frame( [] {
            // Simulation::getInstance()->stop();
            // } );
        }

        ImGui::EndMenu();
    }

    if ( ImGuiFileDialog::Instance()->Display( "LoadSceneDlgKey" ) ) {
        if ( ImGuiFileDialog::Instance()->IsOk() ) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            crimild::editor::loadScene( filePathName );
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if ( ImGuiFileDialog::Instance()->Display( "SaveSceneDlgKey" ) ) {
        if ( ImGuiFileDialog::Instance()->IsOk() ) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            crimild::editor::saveSceneAs( filePathName );
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if ( ImGuiFileDialog::Instance()->Display( "ExportSceneDlgKey" ) ) {
        if ( ImGuiFileDialog::Instance()->IsOk() ) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            crimild::editor::exportScene( filePathName );
        }
        ImGuiFileDialog::Instance()->Close();
    }

    if ( ImGuiFileDialog::Instance()->Display( "ChooseFileDlgKey" ) ) {
        if ( ImGuiFileDialog::Instance()->IsOk() ) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            crimild::editor::importFile( filePathName );
        }
        ImGuiFileDialog::Instance()->Close();
    }
}
