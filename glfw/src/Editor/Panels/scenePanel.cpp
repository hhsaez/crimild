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

#include "Editor/Panels/scenePanel.hpp"

#include "Editor/EditorLayer.hpp"
#include "Foundation/ImGUIUtils.hpp"
#include "Mathematics/Matrix4_constants.hpp"
#include "Mathematics/Matrix4_inverse.hpp"
#include "Rendering/VulkanFramebufferAttachment.hpp"
#include "SceneGraph/Camera.hpp"
#include "Simulation/Input.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

static bool scenePanelVisible = true;

void drawGizmo( Node *selectedNode, float x, float y, float width, float height )
{
    if ( selectedNode == nullptr ) {
        return;
    }

    static ImGuizmo::OPERATION gizmoOperation( ImGuizmo::TRANSLATE );
    static ImGuizmo::MODE gizmoMode( ImGuizmo::LOCAL );
    if ( ImGui::IsKeyPressed( CRIMILD_INPUT_KEY_W ) )
        gizmoOperation = ImGuizmo::TRANSLATE;
    if ( ImGui::IsKeyPressed( CRIMILD_INPUT_KEY_E ) )
        gizmoOperation = ImGuizmo::ROTATE;
    if ( ImGui::IsKeyPressed( CRIMILD_INPUT_KEY_R ) ) // r Key
        gizmoOperation = ImGuizmo::SCALE;

    ImGuizmo::BeginFrame();

    ImGuizmo::SetOrthographic( false );
    // TODO: Set this to nullptr if we are drawing inside a window
    // Otherwise, use ImGui::GetForegroundDrawList() to draw in the whole screen.
    ImGuizmo::SetDrawlist( ImGui::GetForegroundDrawList() );
    ImGuiIO &io = ImGui::GetIO();
    ImGuizmo::SetRect( x, y, width, height );

    const auto [ view, proj ] = [] {
        if ( auto camera = Camera::getMainCamera() ) {
            return std::make_pair( camera->getViewMatrix(), camera->getProjectionMatrix() );
        }
        return std::make_pair( Matrix4::Constants::IDENTITY, Matrix4::Constants::IDENTITY );
    }();

    // TODO: Snapping
    bool snap = false;
    // TODO: Snapping values should depend on the mode (i.e. meters, degrees, etc.)
    const auto snapValues = Vector3 { 1, 1, 1 };

    auto res = selectedNode->getLocal().mat;
    ImGuizmo::Manipulate(
        static_cast< const float * >( &view.c0.x ),
        static_cast< const float * >( &proj.c0.x ),
        gizmoOperation,
        gizmoMode,
        static_cast< float * >( &res.c0.x ),
        nullptr,
        snap ? static_cast< const float * >( &snapValues.x ) : nullptr );

    selectedNode->setLocal( Transformation { res, inverse( res ) } );
    selectedNode->perform( UpdateWorldState() );
}

void crimild::editor::setScenePanelVisible( bool visible ) noexcept
{
    scenePanelVisible = visible;
}

void crimild::editor::renderScenePanel( EditorLayer *editor ) noexcept
{
    if ( !scenePanelVisible ) {
        return;
    }

    static size_t selectedRenderMode = 0;

    ImGui::SetNextWindowPos( ImVec2( 310, 25 ), ImGuiCond_Always );
    ImGui::SetNextWindowSize( ImVec2( 1280, 720 ), ImGuiCond_Always );

    if ( ImGui::Begin( "Scene", &scenePanelVisible ) ) {
        std::vector< const char * > attachments;
        editor->eachSceneAttachment(
            [ & ]( const auto att ) {
                attachments.push_back( att->name.c_str() );
            } );

        if ( !attachments.empty() ) {
            if ( ImGui::BeginCombo( "Select render mode", attachments[ 0 ], 0 ) ) {
                for ( size_t i = 0; i < attachments.size(); ++i ) {
                    if ( ImGui::Selectable( attachments[ i ], selectedRenderMode == i ) ) {
                        selectedRenderMode = i;
                    }
                }
                ImGui::EndCombo();
            }

            ImTextureID tex_id = ( ImTextureID )( intptr_t )( 1 + selectedRenderMode );
            ImVec2 uv_min = ImVec2( 0.0f, 0.0f );                 // Top-left
            ImVec2 uv_max = ImVec2( 1.0f, 1.0f );                 // Lower-right
            ImVec4 tint_col = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );   // No tint
            ImVec4 border_col = ImVec4( 1.0f, 1.0f, 1.0f, 0.0f ); // 50% opaque white
            ImGui::Image( tex_id, ImVec2( 1260, 660 ), uv_min, uv_max, tint_col, border_col );

        } else {
            ImGui::Text( "No scene attachments found" );
        }

        ImGui::End();
    }

    drawGizmo( editor->getSelectedNode(), 310, 25, 1280, 720 );
}