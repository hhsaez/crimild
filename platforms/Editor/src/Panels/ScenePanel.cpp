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

#include "Concurrency/debounce.hpp"
#include "Foundation/ImGuiUtils.hpp"
#include "Rendering/FrameGraph/VulkanComputeImageFromChannels.hpp"
#include "Rendering/FrameGraph/VulkanRenderScene.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderTarget.hpp"
#include "Rendering/VulkanSynchronization.hpp"
#include "Simulation/Editor.hpp"

using namespace crimild;
using namespace crimild::vulkan::framegraph;
using namespace crimild::editor::panels;

void drawGizmo(
    Node *selectedNode,
    Camera *camera,
    float x,
    float y,
    float width,
    float height,
    bool canInteract
)
{
    if ( selectedNode == nullptr ) {
        return;
    }

    static ImGuizmo::OPERATION gizmoOperation( ImGuizmo::TRANSLATE );
    static ImGuizmo::MODE gizmoMode( ImGuizmo::WORLD );

    if ( canInteract && ImGui::IsWindowFocused() ) {
        if ( ImGui::IsKeyPressed( ImGuiKey( CRIMILD_INPUT_KEY_W ) ) )
            gizmoOperation = ImGuizmo::TRANSLATE;
        if ( ImGui::IsKeyPressed( ImGuiKey( CRIMILD_INPUT_KEY_E ) ) )
            gizmoOperation = ImGuizmo::ROTATE;
        if ( ImGui::IsKeyPressed( ImGuiKey( CRIMILD_INPUT_KEY_R ) ) )
            gizmoOperation = ImGuizmo::SCALE;
    }

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

Scene::Scene( vulkan::RenderDevice *device ) noexcept
    : WithRenderDevice( device )
{
    const auto N = getRenderDevice()->getInFlightFrameCount();
    m_framegraphs.resize( N );
    m_debugTargets.resize( N );
    m_outputTextures.resize( N );
    for ( int i = 0; i < N; ++i ) {
        auto prefix = StringUtils::toString( i );
        auto framegraph = crimild::alloc< vulkan::framegraph::RenderScene >(
            device,
            prefix + "/RenderScene",
            VkExtent2D {
                // Render using a squared aspect ratio, so we don't need to
                // resize images when windows does. The end result might be
                // a bit pixelated, but it's faster. And we can always render to
                // a bigger buffer
                .width = 1280,
                .height = 1280,
            }
        );
        m_framegraphs[ i ] = framegraph;
        m_outputTextures[ i ].push_back(
            crimild::alloc< ImGuiVulkanTexture >(
                "Scene",
                framegraph->getOutput()->getImageView(),
                framegraph->getOutput()->getSampler()
            )
        );

        m_debugTargets[ i ] = {
            // crimild::alloc< vulkan::framegraph::ComputeImageFromChannels >(
            //     device,
            //     "Scene/Depth",
            //     m_framegraphs[ i ]->getRenderTarget( "Scene/Depth" ),
            //     "depth"
            // )
            crimild::alloc< ComputeImageFromChannels >( device, "Albedo", m_framegraphs[ i ]->getRenderTarget( m_framegraphs[ i ]->getName() + "/Targets/Albedo" ), "rgb" ),
            crimild::alloc< ComputeImageFromChannels >( device, "Position", m_framegraphs[ i ]->getRenderTarget( m_framegraphs[ i ]->getName() + "/Targets/Position" ), "rgb" ),
            crimild::alloc< ComputeImageFromChannels >( device, "Normal", m_framegraphs[ i ]->getRenderTarget( m_framegraphs[ i ]->getName() + "/Targets/Normal" ), "rgb" ),
        };

        for ( auto &debugTarget : m_debugTargets[ i ] ) {
            m_outputTextures[ i ].push_back(
                crimild::alloc< ImGuiVulkanTexture >(
                    debugTarget->getName(),
                    debugTarget->getOutput()->getImageView(),
                    debugTarget->getOutput()->getSampler()
                )
            );
        }
    }

    m_editorCamera = std::make_unique< Camera >();
    m_cameraTranslation = translation( 10, 10, 10 );
    m_cameraRotation = euler( radians( 45 ), radians( -35 ), 0 );
    m_editorCamera->setWorld( m_cameraTranslation * m_cameraRotation );
}

Event Scene::handle( const Event &e ) noexcept
{
    return e;
}

void Scene::onRender( void ) noexcept
{
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 renderSize = ImGui::GetContentRegionAvail();
    bool isMinimized = renderSize.x < 1 || renderSize.y < 1;
    m_extent.width = renderSize.x;
    m_extent.height = renderSize.y;

    auto currentFrameIdx = getRenderDevice()->getCurrentFrameIndex();
    if ( !m_outputTextures.empty() ) {
        auto &textures = m_outputTextures[ currentFrameIdx ];
        auto tex_id = textures[ m_selectedTexture ]->getDescriptorSet();
        ImVec2 uv_min = ImVec2( 0.0f, 0.0f );                 // Top-left
        ImVec2 uv_max = ImVec2( 1.0f, 1.0f );                 // Lower-right
        ImVec4 tint_col = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );   // No tint
        ImVec4 border_col = ImVec4( 1.0f, 1.0f, 1.0f, 0.0f ); // 50% opaque white
        ImGui::Image( tex_id, renderSize, uv_min, uv_max, tint_col, border_col );

        ImGui::SetCursorPos( ImVec2( 20, 40 ) );
        ImGui::BeginChild( "Settings", ImVec2( 200, 200 ) );
        static ImGuiComboFlags flags = 0;
        if ( ImGui::BeginCombo( "##scenePanelOutput", textures[ m_selectedTexture ]->getName().c_str(), flags ) ) {
            for ( size_t i = 0; i < textures.size(); ++i ) {
                const auto &texture = textures[ i ];
                const auto isSelected = i == m_selectedTexture;
                if ( ImGui::Selectable( texture->getName().c_str(), isSelected ) ) {
                    m_selectedTexture = i;
                }
                if ( isSelected ) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::EndChild();
    } else {
        ImGui::Text( "No scene attachments found" );
    }

    if ( isMinimized ) {
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
                m_extent.height,
                !m_editorCameraEnabled
            );
        }
    }

    updateCamera();

    std::vector< std::shared_ptr< vulkan::Semaphore > > signals;
    for ( auto &debugTarget : m_debugTargets[ currentFrameIdx ] ) {
        signals.push_back( debugTarget->getSemaphore() );
    }
    m_framegraphs[ currentFrameIdx ]->render(
        crimild::Simulation::getInstance()->getScene(),
        m_editorCamera.get(),
        {
            .signal = signals,
        }
    );

    for ( auto &debugTarget : m_debugTargets[ currentFrameIdx ] ) {
        debugTarget->execute(
            {
                .pre = {
                    // Transition target image to general layout so it can be written
                    .imageMemoryBarriers = {
                        vulkan::ImageMemoryBarrier {
                            .srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                            .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                            .srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
                            .dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
                            .oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                            .newLayout = VK_IMAGE_LAYOUT_GENERAL,
                            .imageView = debugTarget->getOutput()->getImageView(),
                        },
                    },
                },
                .post = {
                    // Transition target image to shader read
                    .imageMemoryBarriers = {
                        vulkan::ImageMemoryBarrier {
                            .srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                            .dstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                            .srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
                            .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                            .oldLayout = VK_IMAGE_LAYOUT_GENERAL,
                            .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                            .imageView = debugTarget->getOutput()->getImageView(),
                        },
                    },
                },
                .wait = { debugTarget->getSemaphore() },
            }
        );
    }
}

void Scene::updateCamera( void ) noexcept
{
    // Update editor camera aspect ratio based on current window's size
    m_editorCamera->setAspectRatio( m_extent.width / m_extent.height );

    ImVec2 mousePositionAbsolute = ImGui::GetMousePos();
    ImVec2 screenPositionAbsolute = ImGui::GetItemRectMin();
    ImVec2 mousePositionRelative = ImVec2( mousePositionAbsolute.x - screenPositionAbsolute.x, mousePositionAbsolute.y - screenPositionAbsolute.y );
    auto mousePos = Vector2 { mousePositionRelative.x, mousePositionRelative.y };
    auto delta = mousePos - m_lastMousePos;

    if ( ImGui::IsMouseDown( ImGuiMouseButton_Right ) ) {
        if ( ImGui::IsWindowFocused() && ImGui::IsWindowHovered() ) {
            Input::getInstance()->setMouseCursorMode( Input::MouseCursorMode::GRAB );
            m_editorCameraEnabled = true;
        }
    } else {
        Input::getInstance()->setMouseCursorMode( Input::MouseCursorMode::NORMAL );
        m_editorCameraEnabled = false;
    }

    if ( m_editorCameraEnabled ) {
        if ( !isZero( delta[ 1 ] ) ) {
            const auto R = right( m_cameraRotation );
            m_cameraRotation = rotation( R, -0.005 * delta[ 1 ] ) * m_cameraRotation;
        }

        if ( !isZero( delta[ 0 ] ) ) {
            // This leads to gimbal lock when looking straight up/down, but I'm ok with it for now
            const auto U = Vector3 { 0, 1, 0 };
            m_cameraRotation = rotation( U, -0.005 * delta[ 0 ] ) * m_cameraRotation;
        }

        const auto F = forward( m_cameraRotation );
        const auto R = right( m_cameraRotation );
        if ( ImGui::IsKeyDown( ImGuiKey_W ) ) {
            m_cameraTranslation = translation( F ) * m_cameraTranslation;
            m_editorCamera->setWorld( m_cameraTranslation * m_cameraRotation );
        }
        if ( ImGui::IsKeyDown( ImGuiKey_S ) ) {
            m_cameraTranslation = translation( -F ) * m_cameraTranslation;
            m_editorCamera->setWorld( m_cameraTranslation * m_cameraRotation );
        }
        if ( ImGui::IsKeyDown( ImGuiKey_A ) ) {
            m_cameraTranslation = translation( -R ) * m_cameraTranslation;
            m_editorCamera->setWorld( m_cameraTranslation * m_cameraRotation );
        }
        if ( ImGui::IsKeyDown( ImGuiKey_D ) ) {
            m_cameraTranslation = translation( R ) * m_cameraTranslation;
            m_editorCamera->setWorld( m_cameraTranslation * m_cameraRotation );
        }
    }

    m_lastMousePos = mousePos;
    m_editorCamera->setWorld( m_cameraTranslation * m_cameraRotation );
    m_editorCamera->setWorldIsCurrent( true );
}
