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

#include "Editor/Panels/ScenePanel.hpp"

#include "Editor/EditorLayer.hpp"
#include "Foundation/ImGUIUtils.hpp"
#include "Mathematics/Matrix4_inverse.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "SceneGraph/Camera.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;
using namespace crimild::editor;

bool ScenePanel::s_visible = true;

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

ScenePanel::ScenePanel( vulkan::RenderDevice *renderDevice ) noexcept
    : RenderPass( renderDevice ),
      m_gBufferPass( renderDevice ),
      m_localLightingPass(
          renderDevice,
          m_gBufferPass.getAlbedoAttachment(),
          m_gBufferPass.getPositionAttachment(),
          m_gBufferPass.getNormalAttachment(),
          m_gBufferPass.getMaterialAttachment(),
          // TODO: Use actual shadow map
          m_gBufferPass.getAlbedoAttachment() )
{
    // no-op
}

Event ScenePanel::handle( const Event &e ) noexcept
{
    m_gBufferPass.handle( e );
    m_localLightingPass.handle( e );
    return e;
}

void ScenePanel::render( void ) noexcept
{
    m_gBufferPass.render();

    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    const auto attachments = std::array< const vulkan::FramebufferAttachment *, 5 > {
        m_gBufferPass.getAlbedoAttachment(),
        m_gBufferPass.getPositionAttachment(),
        m_gBufferPass.getNormalAttachment(),
        m_gBufferPass.getMaterialAttachment(),
        m_gBufferPass.getDepthStencilAttachment(),
    };

    for ( const auto att : attachments ) {
        getRenderDevice()->transitionImageLayout(
            commandBuffer,
            att->image,
            att->format,
            getRenderDevice()->formatIsColor( att->format ) ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            att->mipLevels,
            att->layerCount );
    }

    m_localLightingPass.render();

    getRenderDevice()->transitionImageLayout(
        commandBuffer,
        m_localLightingPass.getColorAttachment()->image,
        m_localLightingPass.getColorAttachment()->format,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        m_localLightingPass.getColorAttachment()->mipLevels,
        m_localLightingPass.getColorAttachment()->layerCount );
}

void ScenePanel::updateUI( EditorLayer *editor, bool ) noexcept
{
    if ( !isVibisle() ) {
        return;
    }

    const auto attachments = std::array< const vulkan::FramebufferAttachment *, 6 > {
        m_localLightingPass.getColorAttachment(),
        m_gBufferPass.getAlbedoAttachment(),
        m_gBufferPass.getPositionAttachment(),
        m_gBufferPass.getNormalAttachment(),
        m_gBufferPass.getMaterialAttachment(),
        m_gBufferPass.getDepthStencilAttachment(),
    };

    static size_t selectedRenderMode = 0;
    if ( ImGui::BeginCombo( "Select render mode", attachments[ selectedRenderMode ]->name.c_str(), 0 ) ) {
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

    drawGizmo( editor->getSelectedNode(), 310, 25, 1280, 720 );
}
