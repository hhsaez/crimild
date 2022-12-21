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
#include "Concurrency/Async.hpp"
#include "Editor/EditorLayer.hpp"
#include "Editor/EditorUtils.hpp"
#include "Foundation/ImGUIUtils.hpp"
#include "Foundation/Version.hpp"

#include <filesystem>
#include <iostream>

void crimild::editor::fileMenu( void ) noexcept
{
    static std::string dialogId;
    static std::function< void( const std::filesystem::path & ) > dialogHandler;

    auto openDialog = [ & ]( std::string id, std::string title, std::function< void( std::string ) > handler, const char *filters = ".crimild" ) {
        dialogId = id;
        dialogHandler = handler;
        ImGuiFileDialogFlags flags = ImGuiFileDialogFlags_Default;
        ImGuiFileDialog::Instance()->OpenDialog( id, title, filters, ".", 1, nullptr, flags );
    };

    if ( ImGui::BeginMenu( "File" ) ) {
        if ( ImGui::MenuItem( "New Project..." ) ) {
            openDialog(
                "NewProjectDlgKey",
                "Create Project",
                []( const auto &path ) {
                    EditorLayer::getInstance()->createProject( path );
                },
                nullptr
            );
        }

        if ( ImGui::MenuItem( "New Scene..." ) ) {
            openDialog(
                "NewSceneDlgKey",
                "Create Scene",
                []( const auto &path ) {
                    EditorLayer::getInstance()->createNewScene( path );
                },
                ".crimild"
            );
        }

        ImGui::Separator();

        if ( ImGui::MenuItem( "Open Project..." ) ) {
            openDialog(
                "OpenProjectDlgKey",
                "Open Project",
                []( const auto &path ) {
                    EditorLayer::getInstance()->loadProject( path );
                },
                nullptr
            );
        }

        if ( ImGui::MenuItem( "Open Scene..." ) ) {
            openDialog(
                "OpenSceneDlgKey",
                "Open Scene",
                []( const auto &path ) {
                    EditorLayer::getInstance()->loadScene( path );
                },
                ".crimild"
            );
        }
        
        ImGui::Separator();

        if ( ImGui::MenuItem( "Save Scene As..." ) ) {
            openDialog(
                "SaveSceneAsDlgKey",
                "Save Scene As...",
                []( const auto &path ) {
                    EditorLayer::getInstance()->saveSceneAs( path );
                },
                ".crimild"
            );
        }
        
        ImGui::Separator();

        if ( ImGui::BeginMenu( "Import..." ) ) {
            if ( ImGui::MenuItem( "glTF 2.0 (.gltf)..." ) ) {
                openDialog(
                    "ImportGLTFDlgKey",
                    "Import",
                    []( const auto &path ) {
                        // TODO
                    },
                    ".gltf"
                );
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if ( ImGui::MenuItem( "Quit" ) ) {
            // crimild::concurrency::sync_frame( [] {
            // Simulation::getInstance()->stop();
            // } );
        }

        ImGui::EndMenu();
    }

    if ( dialogHandler != nullptr && !dialogId.empty() ) {
        ImGui::SetNextWindowSize( ImVec2( 800, 600 ) );
        if ( ImGuiFileDialog::Instance()->Display( dialogId ) ) {
            if ( ImGuiFileDialog::Instance()->IsOk() ) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                const auto path = std::filesystem::path { filePathName };
                // Resolve at the beginning of next frame
                crimild::concurrency::sync_frame(
                    [ path, handler = dialogHandler ] {
                        handler( path );
                    }
                );
            }
            ImGuiFileDialog::Instance()->Close();
            dialogHandler = nullptr;
            dialogId = "";
        }
    }
}
