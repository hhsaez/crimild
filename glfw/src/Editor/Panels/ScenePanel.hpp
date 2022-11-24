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

#ifndef CRIMILD_EDITOR_PANELS_SCENE_
#define CRIMILD_EDITOR_PANELS_SCENE_

#include "Editor/Layout.hpp"
#include "Mathematics/Point2.hpp"
#include "Mathematics/Transformation_constants.hpp"
#include "Rendering/RenderPasses/VulkanDebugAttachmentPass.hpp"
#include "Rendering/RenderPasses/VulkanOverlayPass.hpp"
#include "Rendering/RenderPasses/VulkanSceneDebugPass.hpp"
#include "Rendering/RenderPasses/VulkanScenePass.hpp"
#include "SceneGraph/Camera.hpp"

namespace crimild {

    class Camera;

    namespace vulkan {

        class RenderDevice;

    }

    namespace editor {

        class ScenePanel : public layout::Panel {
        public:
            ScenePanel( vulkan::RenderDevice *renderDevice ) noexcept;
            virtual ~ScenePanel( void ) = default;

            virtual Event handle( const Event &e ) noexcept override;
            virtual void render( void ) noexcept override;

            inline const vulkan::RenderDevice *getRenderDevice( void ) const noexcept { return m_renderDevice; }

        private:
            vulkan::RenderDevice *m_renderDevice = nullptr;

            Extent2D m_extent = Extent2D { .width = 1280.0, .height = 695.0 };
            Event m_lastResizeEvent = Event {};
            vulkan::ScenePass m_scenePass;

            vulkan::SceneDebugPass m_sceneDebugPass;
            vulkan::OverlayPass m_sceneDebugOverlayPass;
            std::vector< SharedPointer< vulkan::DebugAttachmentPass > > m_debugPasses;

            std::unique_ptr< Camera > m_editorCamera;
            bool m_editorCameraEnabled = false;
            Vector2i m_lastMousePos = Vector2i { 0, 0 };
            Transformation m_cameraRotation = Transformation::Constants::IDENTITY;
            Transformation m_cameraTranslation = Transformation::Constants::IDENTITY;

            std::vector< const vulkan::FramebufferAttachment * > m_attachments;
            size_t m_selectedAttachment = 0;
        };

    }

}

#endif
