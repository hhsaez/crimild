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

#include "Views/Panels/Scene3D/Scene3DPanel.hpp"

#include "Foundation/ImGuiUtils.hpp"
#include "Rendering/FrameGraph/VulkanComputeImageMix.hpp"
#include "Rendering/FrameGraph/VulkanComputeImageSwizzle.hpp"
#include "Rendering/FrameGraph/VulkanRenderScene.hpp"
#include "Rendering/FrameGraph/VulkanRenderSceneDebug.hpp"
#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderTarget.hpp"
#include "Rendering/VulkanSampler.hpp"
#include "Rendering/VulkanSynchronization.hpp"

using namespace crimild;
using namespace crimild::editor;
using namespace crimild::vulkan::framegraph;

static void drawGizmo(
   crimild::Node *selectedNode,
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

   auto newWorld = Matrix4( selectedNode->getWorld() );
   ImGuizmo::Manipulate(
      static_cast< const float * >( &view[ 0 ].x ),
      static_cast< const float * >( &proj[ 0 ].x ),
      gizmoOperation,
      gizmoMode,
      static_cast< float * >( &newWorld[ 0 ].x ),
      nullptr,
      snap ? static_cast< const float * >( &snapValues.x ) : nullptr
   );

   auto transformFromMatrix = []( const Matrix4 &m ) {
      Transformation t;
      t.scale.x = length( m * Vector4( 1, 0, 0, 0 ) );
      t.scale.y = length( m * Vector4( 0, 1, 0, 0 ) );
      t.scale.z = length( m * Vector4( 0, 0, 1, 0 ) );

      t.translate.x = m[ 3 ].x;
      t.translate.y = m[ 3 ].y;
      t.translate.z = m[ 3 ].z;

      return t;
   };

   if ( selectedNode->hasParent() ) {
      // Node has a parent, so convert world transform into local transform
      const auto newLocal = Matrix4( ( selectedNode->getParent()->getWorld() ) ) * newWorld;
      selectedNode->setLocal( transformFromMatrix( newLocal ) );
      // selectedNode->setLocal( Transformation { newLocal, inverse( newLocal ) } );
      // assert( false && "FIX ME!" );
   } else {
      // Node has no parent, so local transform is same as world
      selectedNode->setLocal( transformFromMatrix( newWorld ) );
      // selectedNode->setLocal( Transformation { newWorld, inverse( newWorld ) } );
      // assert( false && "FIX ME!" );
   }

   selectedNode->perform( UpdateWorldState() );
}

Scene3DPanel::Scene3DPanel( void ) noexcept
   : View( "Scene 3D" )
{
   // no-op
}

void Scene3DPanel::initialize( void ) noexcept
{
   auto device = vulkan::RenderDevice::getInstance();

   m_editorCamera = crimild::alloc< Camera >();
   m_cameraTranslation = translation( 10, 10, 10 );
   m_cameraRotation = Transformation {
      .rotate = euler( radians( 45 ), radians( -35 ), 0 ),
   };
   m_editorCamera->setWorld( m_cameraTranslation( m_cameraRotation ) );

   const auto N = device->getInFlightFrameCount();

   const auto extent = VkExtent2D {
      // Render using a squared aspect ratio, so we don't need to
      // resize images when windows does. The end result might be
      // a bit pixelated, but it's faster. And we can always render to
      // a bigger buffer
      .width = 1280,
      .height = 1280,
   };

   auto createImageView = [ & ]( std::string name, const VkExtent2D &extent, VkFormat format, VkImageUsageFlags usage ) {
      return crimild::alloc< vulkan::ImageView >(
         device,
         name + "/ImageView",
         [ & ] {
            auto image = crimild::alloc< vulkan::Image >(
               device,
               extent,
               format,
               usage,
               name + "/Image"
            );
            image->allocateMemory();
            return image;
         }()
      );
   };

   auto createColorImageView = [ & ]( std::string name, const VkExtent2D &extent, VkFormat format ) {
      return createImageView(
         name,
         extent,
         format,
         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT
      );
   };

   m_framegraph.resize( N );
   m_outputTextures.resize( N );
   m_renderScenePasses.resize( N );
   for ( int i = 0; i < N; ++i ) {
      auto renderSceneOutput = createColorImageView( "Scene/Output", extent, VK_FORMAT_R32G32B32A32_SFLOAT );
      auto renderSceneSemaphore = crimild::alloc< vulkan::Semaphore >( device, "Scene/Semaphore", VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );
      auto renderScene = crimild::alloc< RenderScene >(
         device,
         "Scene",
         m_editorCamera,
         renderSceneOutput,
         vulkan::SyncOptions {
            // .signal = { renderSceneSemaphore },
         }
      );
      m_renderScenePasses[ i ] = renderScene;

      auto renderGizmosOutput = createColorImageView( "Gizmos/Output", extent, VK_FORMAT_R32G32B32A32_SFLOAT );
      auto renderGizmosSemaphore = crimild::alloc< vulkan::Semaphore >( device, "Gizmos/Semaphore", VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT );
      auto renderGizmos = crimild::alloc< RenderSceneDebug >(
         device,
         "Gizmos",
         m_editorCamera,
         renderGizmosOutput,
         vulkan::SyncOptions {
            // .signal = { renderGizmosSemaphore },
         }
      );

      auto mixSceneAndGizmosOutput = createColorImageView( "MixSceneAndGizmos/Output", extent, VK_FORMAT_R32G32B32A32_SFLOAT );
      auto mixSceneAndGizmos = crimild::alloc< ComputeImageMix >(
         device,
         "MixSceneAndGizmos",
         renderGizmosOutput,
         renderSceneOutput,
         mixSceneAndGizmosOutput,
         ComputeImageMix::Mode::NORMAL,
         vulkan::SyncOptions {
            // .wait = { renderSceneSemaphore, renderGizmosSemaphore },
         }
      );

      m_framegraph[ i ] = {
         renderScene,
         renderGizmos,
         mixSceneAndGizmos,
      };

      m_outputTextures[ i ] = {
         crimild::alloc< ImGuiVulkanTexture >( "Default", mixSceneAndGizmosOutput ),
         crimild::alloc< ImGuiVulkanTexture >( "Gizmos Only", renderGizmosOutput ),
         crimild::alloc< ImGuiVulkanTexture >( "Scene Only", renderSceneOutput ),
      };

      auto computeDebugImage = [ & ]( std::string name, std::shared_ptr< vulkan::ImageView > const &imageView, ComputeImageSwizzle::Selector selector ) {
         auto output = createColorImageView( name + "/Output", extent, VK_FORMAT_R32G32B32A32_SFLOAT );
         auto swizzle = crimild::alloc< ComputeImageSwizzle >( device, name, imageView, selector, output );
         m_framegraph[ i ].push_back( swizzle );
         m_outputTextures[ i ].push_back(
            crimild::alloc< ImGuiVulkanTexture >(
               name,
               output
            )
         );
      };

      // computeDebugImage( "Depth", renderScene->getRenderTarget( renderScene->getName() + "/Targets/Depth" )->getImageView(), ComputeImageSwizzle::Selector::RGB );
      computeDebugImage( "Albedo", renderScene->getRenderTarget( renderScene->getName() + "/Targets/Albedo" )->getImageView(), ComputeImageSwizzle::Selector::RGB );
      computeDebugImage( "Position", renderScene->getRenderTarget( renderScene->getName() + "/Targets/Position" )->getImageView(), ComputeImageSwizzle::Selector::RGB );
      computeDebugImage( "Normal", renderScene->getRenderTarget( renderScene->getName() + "/Targets/Normal" )->getImageView(), ComputeImageSwizzle::Selector::NORMAL );
      computeDebugImage( "Metallic", renderScene->getRenderTarget( renderScene->getName() + "/Targets/Material" )->getImageView(), ComputeImageSwizzle::Selector::RRR );
      computeDebugImage( "Roughness", renderScene->getRenderTarget( renderScene->getName() + "/Targets/Material" )->getImageView(), ComputeImageSwizzle::Selector::GGG );
      computeDebugImage( "Ambient Occlusion", renderScene->getRenderTarget( renderScene->getName() + "/Targets/Material" )->getImageView(), ComputeImageSwizzle::Selector::BBB );
   }

   m_initialized = true;
}

void Scene3DPanel::draw( void ) noexcept
{
   if ( !isInitialized() ) {
      initialize();
   }

   ImVec2 windowPos = ImGui::GetWindowPos();
   ImVec2 renderSize = ImGui::GetContentRegionAvail();
   bool isMinimized = renderSize.x < 1 || renderSize.y < 1;
   m_extent.width = renderSize.x;
   m_extent.height = renderSize.y;

   auto currentFrameIdx = vulkan::RenderDevice::getInstance()->getCurrentFrameIndex();
   if ( !m_outputTextures.empty() ) {
      auto &textures = m_outputTextures[ currentFrameIdx ];
      auto tex_id = textures[ m_selectedTexture ]->getDescriptorSet();
      ImVec2 uv_min = ImVec2( 0.0f, 0.0f );                 // Top-left
      ImVec2 uv_max = ImVec2( 1.0f, 1.0f );                 // Lower-right
      ImVec4 tint_col = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );   // No tint
      ImVec4 border_col = ImVec4( 1.0f, 1.0f, 1.0f, 0.0f ); // 50% opaque white
      ImGui::Image( tex_id, renderSize, uv_min, uv_max, tint_col, border_col );

      ImGui::SetCursorPos( ImVec2( 20, 40 ) );
      // ImGui::BeginChild( "Settings", ImVec2( 200, 400 ) );
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
      // ImGui::EndChild();
   } else {
      ImGui::Text( "No scene attachments found" );
   }

   if ( isMinimized ) {
      return;
   }

   //    if ( auto editor = Editor::getInstance() ) {
   //       if ( auto selected = editor->getSelectedObject< Node >() ) {
   //          // TODO: Fix gizmo position
   //          drawGizmo(
   //             selected,
   //             m_editorCamera.get(),
   //             windowPos.x,
   //             windowPos.y,
   //             m_extent.width,
   //             m_extent.height,
   //             !m_editorCameraEnabled
   //          );
   //       }
   //    }

   updateCamera();

   // Update the scene for each render pass
   // This is definitely not ideal, but it should only happen when we recreate the scene in the assembly
   for ( auto &pass : m_renderScenePasses ) {
      if ( !pass->hasScene() ) {
         auto assembly = getAssembly();
         auto &entities = assembly->getEntities();
         for ( auto &entity : entities ) {
            if ( auto scene = std::dynamic_pointer_cast< crimild::Node >( entity ) ) {
               pass->setScene( scene );
               break;
            }
         }
      }
   }

   for ( auto &node : m_framegraph[ currentFrameIdx ] ) {
      node->execute();
   }
}

void Scene3DPanel::updateCamera( void ) noexcept
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
         m_cameraRotation = rotation( R, -0.005 * delta[ 1 ] )( m_cameraRotation );
      }

      if ( !isZero( delta[ 0 ] ) ) {
         // This leads to gimbal lock when looking straight up/down, but I'm ok with it for now
         const auto U = Vector3 { 0, 1, 0 };
         m_cameraRotation = rotation( U, -0.005 * delta[ 0 ] )( m_cameraRotation );
      }

      const auto F = forward( m_cameraRotation );
      const auto R = right( m_cameraRotation );
      if ( ImGui::IsKeyDown( ImGuiKey_W ) ) {
         m_cameraTranslation = translation( F )( m_cameraTranslation );
         m_editorCamera->setWorld( m_cameraTranslation( m_cameraRotation ) );
      }
      if ( ImGui::IsKeyDown( ImGuiKey_S ) ) {
         m_cameraTranslation = translation( -F )( m_cameraTranslation );
         m_editorCamera->setWorld( m_cameraTranslation( m_cameraRotation ) );
      }
      if ( ImGui::IsKeyDown( ImGuiKey_A ) ) {
         m_cameraTranslation = translation( -R )( m_cameraTranslation );
         m_editorCamera->setWorld( m_cameraTranslation( m_cameraRotation ) );
      }
      if ( ImGui::IsKeyDown( ImGuiKey_D ) ) {
         m_cameraTranslation = translation( R )( m_cameraTranslation );
         m_editorCamera->setWorld( m_cameraTranslation( m_cameraRotation ) );
      }
   }

   m_lastMousePos = mousePos;
   m_editorCamera->setWorld( m_cameraTranslation( m_cameraRotation ) );
   m_editorCamera->setWorldIsCurrent( true );
}
