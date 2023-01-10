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

#include "Editor/Panels/EditorProjectPanel.hpp"

#include "Editor/EditorLayer.hpp"
#include "Editor/EditorProject.hpp"

#include <filesystem>

using namespace crimild;

editor::ProjectPanel::ProjectPanel( void ) noexcept
    : layout::Panel( NAME )
{
    // no-op
}

void editor::ProjectPanel::render( void ) noexcept
{
    bool open = true;
    ImGui::Begin( getName().c_str(), &open, 0 );

    if ( auto project = EditorLayer::getInstance()->getProject() ) {
        const auto path = project->getPath().parent_path();
        if ( std::filesystem::exists( path ) ) {
            // Makes sure the root node is always expanded.
            for ( const auto &entry : std::filesystem::directory_iterator( path ) ) {
                traverse( entry.path() );
            }
        } else {
            ImGui::Text( "Project path not found" );
        }
    } else {
        ImGui::Text( "No Project available" );
    }

    ImGui::End();

    if ( !open ) {
        removeFromParent();
    }
}

void editor::ProjectPanel::traverse( const std::filesystem::path &path ) noexcept
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    if ( std::filesystem::is_directory( path ) ) {
        flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    } else {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    if ( m_selectedPath == path ) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    ImGui::PushStyleColor( ImGuiCol_Text, IM_COL32( 255, 255, 255, 255 ) );

    const auto isOpen = ImGui::TreeNodeEx( path.filename().string().c_str(), flags );

    ImGui::PopStyleColor();

    // Handles selection by using mouse hover+released instead of click
    // because click is triggered on mouse down and will conflict with
    // drag/drop events.
    if ( ImGui::IsItemHovered() ) {
        if ( ImGui::IsMouseReleased( 0 ) ) {
            m_selectedPath = path;
        }
    }

    if ( std::filesystem::is_directory( path ) ) {
        if ( isOpen ) {
            for ( const auto &entry : std::filesystem::directory_iterator( path ) ) {
                traverse( entry.path() );
            }
            // Pop tree state only for directories
            ImGui::TreePop();
        }
    }
}
