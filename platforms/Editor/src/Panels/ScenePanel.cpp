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

#include "Panels/ScenePanel.hpp"

#include "Foundation/ImGuiUtils.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Simulation/Editor.hpp"

using namespace crimild;
using namespace crimild::editor::panels;

void drawGizmo( Node *selectedNode, Camera *camera, float x, float y, float width, float height )
{
    if ( selectedNode == nullptr ) {
        return;
    }

    static ImGuizmo::OPERATION gizmoOperation( ImGuizmo::TRANSLATE );
    static ImGuizmo::MODE gizmoMode( ImGuizmo::WORLD );
    if ( ImGui::IsKeyPressed( ImGuiKey( CRIMILD_INPUT_KEY_W ) ) )
        gizmoOperation = ImGuizmo::TRANSLATE;
    if ( ImGui::IsKeyPressed( ImGuiKey( CRIMILD_INPUT_KEY_E ) ) )
        gizmoOperation = ImGuizmo::ROTATE;
    if ( ImGui::IsKeyPressed( ImGuiKey( CRIMILD_INPUT_KEY_R ) ) )
        gizmoOperation = ImGuizmo::SCALE;

    ImGuizmo::BeginFrame();

    ImGuizmo::SetOrthographic( false );
    // TODO: Set this to nullptr if we are drawing inside a window
    // Otherwise, use ImGui::GetForegroundDrawList() to draw in the whole screen.
    ImGuizmo::SetDrawlist( ImGui::GetForegroundDrawList() );
    ImGuizmo::SetRect( x, y, width, height );

    const auto view = camera->getViewMatrix();
    const auto proj = camera->getProjectionMatrix();

    // TODO: Snapping
    bool snap = false;
    // TODO: Snapping values should depend on the mode (i.e. meters, degrees, etc.)
    const auto snapValues = Vector3 { 1, 1, 1 };

    auto newWorld = selectedNode->getWorld().mat;
    ImGuizmo::Manipulate(
        static_cast< const float * >( &view.c0.x ),
        static_cast< const float * >( &proj.c0.x ),
        gizmoOperation,
        gizmoMode,
        static_cast< float * >( &newWorld.c0.x ),
        nullptr,
        snap ? static_cast< const float * >( &snapValues.x ) : nullptr
    );

    if ( selectedNode->hasParent() ) {
        // Node has a parent, so convert world transform into local transform
        const auto newLocal = selectedNode->getParent()->getWorld().invMat * newWorld;
        selectedNode->setLocal( Transformation { newLocal, inverse( newLocal ) } );
    } else {
        // Node has no parent, so local transform is same as world
        selectedNode->setLocal( Transformation { newWorld, inverse( newWorld ) } );
    }

    selectedNode->perform( UpdateWorldState() );
}

Scene::Scene( vulkan::RenderDevice *renderDevice ) noexcept
    : m_renderDevice( renderDevice ),
      m_scenePass( renderDevice ),
      m_sceneDebugPass( renderDevice ),
      m_sceneDebugOverlayPass(
          renderDevice,
          "Editor",
          {
              m_scenePass.getColorAttachment(),
              m_sceneDebugPass.getColorAttachment(),
          }
      )
{
    m_editorCamera = std::make_unique< Camera >();
    m_cameraTranslation = translation( 10, 10, 10 );
    m_cameraRotation = euler( radians( 45 ), radians( -35 ), 0 );
    m_editorCamera->setWorld( m_cameraTranslation * m_cameraRotation );

    m_debugPasses = {
        crimild::alloc< vulkan::DebugAttachmentPass >( renderDevice, "Scene/Depth", m_scenePass.getAttachment( 0 ), "depth" ),
        crimild::alloc< vulkan::DebugAttachmentPass >( renderDevice, "Scene/Albedo", m_scenePass.getAttachment( 1 ), "rgb" ),
        crimild::alloc< vulkan::DebugAttachmentPass >( renderDevice, "Scene/Position", m_scenePass.getAttachment( 2 ), "rgb" ),
        crimild::alloc< vulkan::DebugAttachmentPass >( renderDevice, "Scene/Normal", m_scenePass.getAttachment( 3 ), "rgb" ),
        crimild::alloc< vulkan::DebugAttachmentPass >( renderDevice, "Scene/Metallic", m_scenePass.getAttachment( 4 ), "r" ),
        crimild::alloc< vulkan::DebugAttachmentPass >( renderDevice, "Scene/Roughness", m_scenePass.getAttachment( 4 ), "g" ),
        crimild::alloc< vulkan::DebugAttachmentPass >( renderDevice, "Scene/Ambient Occlusion", m_scenePass.getAttachment( 4 ), "b" ),
    };

    m_selectedAttachment = 0;
    m_attachments.push_back( m_sceneDebugOverlayPass.getColorAttachment() );
    m_attachments.push_back( m_scenePass.getColorAttachment() );
    for ( const auto &pass : m_debugPasses ) {
        m_attachments.push_back( pass->getColorAttachment() );
    }
    m_attachments.push_back( m_sceneDebugPass.getColorAttachment() );
}

void Scene::render( void ) noexcept
{
    bool open = true;
    const bool visible = ImGui::Begin( "Scene", &open, 0 );

    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 renderSize = ImGui::GetContentRegionAvail();
    bool isMinimized = renderSize.x < 1 || renderSize.y < 1;
    if ( !isMinimized ) {
        if ( m_extent.width != renderSize.x || m_extent.height != renderSize.y ) {
            m_extent.width = renderSize.x;
            m_extent.height = renderSize.y;
            const auto e = Event {
                .type = Event::Type::WINDOW_RESIZE,
                .extent = m_extent,
            };

            m_scenePass.handle( e );
            m_sceneDebugPass.handle( e );
            m_sceneDebugOverlayPass.handle( e );
            for ( auto &pass : m_debugPasses ) {
                pass->handle( e );
            }

            m_descriptorSets.clear();
        }
    }

    uint32_t currentFrameIdx = m_scenePass.getRenderDevice()->getCurrentFrameIndex();

    if ( !m_attachments.empty() ) {
        const auto att = m_attachments[ m_selectedAttachment ];
        if ( !att->descriptorSets.empty() ) {
            if ( !m_descriptorSets.contains( att ) ) {
                auto ds = std::vector< VkDescriptorSet >( m_scenePass.getRenderDevice()->getInFlightFrameCount() );
                for ( int i = 0; i < ds.size(); ++i ) {
                    ds[ i ] = ImGui_ImplVulkan_AddTexture(
                        att->sampler,
                        ( VkImageView ) *att->imageViews[ currentFrameIdx ].get(),
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                    );
                }
                m_descriptorSets[ att ] = ds;
            }
            auto tex_id = m_descriptorSets.at( att )[ currentFrameIdx ];
            ImVec2 uv_min = ImVec2( 0.0f, 0.0f );                 // Top-left
            ImVec2 uv_max = ImVec2( 1.0f, 1.0f );                 // Lower-right
            ImVec4 tint_col = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );   // No tint
            ImVec4 border_col = ImVec4( 1.0f, 1.0f, 1.0f, 0.0f ); // 50% opaque white
            ImGui::Image( tex_id, ImGui::GetContentRegionAvail(), uv_min, uv_max, tint_col, border_col );
        } else {
            ImGui::Text( "No scene attachments found" );
        }

        ImGui::SetCursorPos( ImVec2( 20, 40 ) );
        ImGui::BeginChild( "Settings", ImVec2( 200, 200 ) );
        static ImGuiComboFlags flags = 0;
        if ( ImGui::BeginCombo( "##scenePanelOutput", m_attachments[ m_selectedAttachment ]->name.c_str(), flags ) ) {
            for ( size_t i = 0; i < m_attachments.size(); ++i ) {
                const auto &att = m_attachments[ i ];
                const auto isSelected = i == m_selectedAttachment;
                if ( ImGui::Selectable( att->name.c_str(), isSelected ) ) {
                    m_selectedAttachment = i;
                }
                if ( isSelected ) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::EndChild();
    }

    ImGui::End();

    if ( !visible || isMinimized || !open ) {
        return;
    }

    if ( auto editor = Editor::getInstance() ) {
        if ( auto selected = editor->getSelectedObject< Node >() ) {
            // TODO: Fix gizmo position
            drawGizmo(
                selected,
                m_editorCamera.get(),
                windowPos.x,
                windowPos.y,
                m_extent.width,
                m_extent.height
            );
        }
    }

    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    const auto currentFrameIndex = getRenderDevice()->getCurrentFrameIndex();

    auto transitionAttachment = [ & ]( const auto att ) {
        getRenderDevice()->transitionImageLayout(
            commandBuffer,
            *att->images[ currentFrameIndex ],
            att->format,
            getRenderDevice()->formatIsColor( att->format )
                ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            getRenderDevice()->formatIsColor( att->format )
                ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            att->mipLevels,
            att->layerCount
        );
    };

    auto scene = Simulation::getInstance()->getScene();

    m_scenePass.render( scene, m_editorCamera.get() );

    for ( auto &pass : m_debugPasses ) {
        pass->render( scene, m_editorCamera.get() );
    }

    m_sceneDebugPass.render( scene, m_editorCamera.get() );
    transitionAttachment( m_sceneDebugPass.getColorAttachment() );

    m_sceneDebugOverlayPass.render();
    transitionAttachment( m_sceneDebugOverlayPass.getColorAttachment() );
}
