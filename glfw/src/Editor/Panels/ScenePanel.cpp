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
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_euler.hpp"
#include "Mathematics/Transformation_operators.hpp"
#include "Mathematics/Transformation_rotation.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/Vector2Ops.hpp"
#include "Mathematics/swizzle.hpp"
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
      m_shadowPass( renderDevice ),
      m_shadowDebugPass( renderDevice, "Scene/Shadow (Debug)", m_shadowPass.getShadowAttachment() ),
      m_gBufferPass( renderDevice ),
      m_depthDebugPass( renderDevice, "Scene/Depth (Debug)", m_gBufferPass.getDepthStencilAttachment() ),
      m_localLightingPass(
          renderDevice,
          m_gBufferPass.getAlbedoAttachment(),
          m_gBufferPass.getPositionAttachment(),
          m_gBufferPass.getNormalAttachment(),
          m_gBufferPass.getMaterialAttachment(),
          m_shadowPass.getShadowAttachment() ),
      m_skyboxPass(
          renderDevice,
          m_localLightingPass.getColorAttachment(),
          m_gBufferPass.getDepthStencilAttachment() ),
      m_sceneDebugPass( renderDevice ),
      m_sceneDebugOverlayPass(
          renderDevice,
          "Scene/Debug/Overlay",
          {
              m_localLightingPass.getColorAttachment(),
              m_sceneDebugPass.getColorAttachment(),
          } )
{
    m_editorCamera = std::make_unique< Camera >();
    m_cameraTranslation = translation( 10, 10, 10 );
    m_cameraRotation = euler( radians( 45 ), radians( -30 ), 0 );
    m_editorCamera->setWorld( m_cameraTranslation * m_cameraRotation );
}

Event ScenePanel::handle( const Event &e ) noexcept
{
    switch ( e.type ) {
        case Event::Type::WINDOW_RESIZE:
            m_editorCamera->setAspectRatio( e.extent.width / e.extent.height );
            break;

        case Event::Type::MOUSE_BUTTON_DOWN:
            if ( e.button.button == CRIMILD_INPUT_MOUSE_BUTTON_RIGHT ) {
                Input::getInstance()->setMouseCursorMode( Input::MouseCursorMode::GRAB );
                m_editorCameraEnabled = true;
                return Event {};
            }

        case Event::Type::MOUSE_MOTION: {
            const auto delta = e.motion.pos - m_lastMousePos;
            m_lastMousePos = e.motion.pos;

            if ( m_editorCameraEnabled ) {
                if ( !isZero( delta[ 1 ] ) ) {
                    const auto R = right( m_cameraRotation );
                    m_cameraRotation = rotation( R, -0.005 * delta[ 1 ] ) * m_cameraRotation;
                    m_editorCamera->setWorld( m_cameraTranslation * m_cameraRotation );
                }

                if ( !isZero( delta[ 0 ] ) ) {
                    // This leads to gimbal lock when looking straight up/down, but I'm ok with it for now
                    const auto U = Vector3 { 0, 1, 0 };
                    m_cameraRotation = rotation( U, -0.005 * delta[ 0 ] ) * m_cameraRotation;
                    m_editorCamera->setWorld( m_cameraTranslation * m_cameraRotation );
                }
            }
            break;
        }

        case Event::Type::MOUSE_BUTTON_UP:
            if ( e.button.button == CRIMILD_INPUT_MOUSE_BUTTON_RIGHT ) {
                Input::getInstance()->setMouseCursorMode( Input::MouseCursorMode::NORMAL );
                m_editorCameraEnabled = false;
                return Event {};
            }

        case Event::Type::KEY_DOWN:
        case Event::Type::KEY_REPEAT: {
            // TODO: This makes the camera movement a bit jerky, since KEY_REPEAT events
            // are not continunous. It's ok for now, but I'll probably change it in the
            // future to use TICK events instead.
            if ( m_editorCameraEnabled ) {
                const auto F = forward( m_cameraRotation );
                const auto R = right( m_cameraRotation );

                if ( e.keyboard.key == CRIMILD_INPUT_KEY_W ) {
                    m_cameraTranslation = translation( F ) * m_cameraTranslation;
                    m_editorCamera->setWorld( m_cameraTranslation * m_cameraRotation );
                }

                if ( e.keyboard.key == CRIMILD_INPUT_KEY_S ) {
                    m_cameraTranslation = translation( -F ) * m_cameraTranslation;
                    m_editorCamera->setWorld( m_cameraTranslation * m_cameraRotation );
                }

                if ( e.keyboard.key == CRIMILD_INPUT_KEY_A ) {
                    m_cameraTranslation = translation( -R ) * m_cameraTranslation;
                    m_editorCamera->setWorld( m_cameraTranslation * m_cameraRotation );
                }

                if ( e.keyboard.key == CRIMILD_INPUT_KEY_D ) {
                    m_cameraTranslation = translation( R ) * m_cameraTranslation;
                    m_editorCamera->setWorld( m_cameraTranslation * m_cameraRotation );
                }
            }
            break;
        }

        default:
            break;
    }

    m_shadowPass.handle( e );
    m_shadowDebugPass.handle( e );
    m_gBufferPass.handle( e );
    m_depthDebugPass.handle( e );
    m_localLightingPass.handle( e );
    m_skyboxPass.handle( e );
    m_sceneDebugPass.handle( e );
    m_sceneDebugOverlayPass.handle( e );
    return e;
}

void ScenePanel::render( void ) noexcept
{
    Camera::setMainCamera( m_editorCamera.get() );

    auto commandBuffer = getRenderDevice()->getCurrentCommandBuffer();

    m_shadowPass.render();

    m_gBufferPass.render();

    auto transitionAttachment = [ & ]( const auto att ) {
        getRenderDevice()->transitionImageLayout(
            commandBuffer,
            att->image,
            att->format,
            getRenderDevice()->formatIsColor( att->format )
                ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            getRenderDevice()->formatIsColor( att->format )
                ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                : VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
            att->mipLevels,
            att->layerCount );
    };

    const auto attachments = std::vector< const vulkan::FramebufferAttachment * > {
        m_gBufferPass.getAlbedoAttachment(),
        m_gBufferPass.getPositionAttachment(),
        m_gBufferPass.getNormalAttachment(),
        m_gBufferPass.getMaterialAttachment(),
    };

    for ( const auto att : attachments ) {
        transitionAttachment( att );
    }

    m_localLightingPass.render();
    m_skyboxPass.render();

    // transitionAttachment( m_skyboxPass.getColorAttachment() );

    // Accumulated color buffer can be transitioned now
    transitionAttachment( m_localLightingPass.getColorAttachment() );

    // Depth buffer can be transition now
    transitionAttachment( m_gBufferPass.getDepthStencilAttachment() );

    if ( auto camera = Camera::getMainCamera() ) {
        m_depthDebugPass.setNear( camera->getNear() );
        m_depthDebugPass.setFar( camera->getFar() );
    }
    m_depthDebugPass.render();
    transitionAttachment( m_depthDebugPass.getColorAttachment() );

    m_shadowDebugPass.render();
    transitionAttachment( m_shadowDebugPass.getColorAttachment() );

    m_sceneDebugPass.render();
    transitionAttachment( m_sceneDebugPass.getColorAttachment() );

    m_sceneDebugOverlayPass.render();
    transitionAttachment( m_sceneDebugOverlayPass.getColorAttachment() );
}

void ScenePanel::updateUI( EditorLayer *editor, bool ) noexcept
{
    if ( !isVibisle() ) {
        return;
    }

    const auto attachments = std::vector< const vulkan::FramebufferAttachment * > {
        m_skyboxPass.getColorAttachment(),
        m_localLightingPass.getColorAttachment(),
        m_gBufferPass.getAlbedoAttachment(),
        m_gBufferPass.getPositionAttachment(),
        m_gBufferPass.getNormalAttachment(),
        m_gBufferPass.getMaterialAttachment(),
        m_depthDebugPass.getColorAttachment(),
        m_shadowDebugPass.getColorAttachment(),
        m_sceneDebugPass.getColorAttachment(),
        m_sceneDebugOverlayPass.getColorAttachment(),
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
