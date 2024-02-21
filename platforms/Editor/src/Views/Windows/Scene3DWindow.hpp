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

#ifndef CRIMILD_EDITOR_VIEWS_WINDOWS_SCENE_3D_
#define CRIMILD_EDITOR_VIEWS_WINDOWS_SCENE_3D_

#include "Foundation/VulkanUtils.hpp"
#include "Mathematics/Transformation_constants.hpp"
#include "Views/Windows/Window.hpp"

#include <unordered_map>

namespace crimild {

    class Node;
    class Camera;

    namespace vulkan::framegraph {

        class Node;

    }

}

namespace crimild::editor {

    class Scene3DWindow : public Window {
        CRIMILD_IMPLEMENT_RTTI( crimild::editor::Scene3DWindow )

    public:
        Scene3DWindow( void ) noexcept;
        ~Scene3DWindow( void ) noexcept = default;

        void drawContent( void ) noexcept final;

    private:
        void initialize( void ) noexcept;
        [[nodiscard]] inline bool isInitialized( void ) const noexcept { return m_initialized; }

        void updateCamera( void ) noexcept;

    private:
        bool m_initialized = false;

        Extent2D m_extent = Extent2D { .width = 1280.0, .height = 1280.0 };

        std::vector< std::vector< std::shared_ptr< vulkan::framegraph::Node > > > m_framegraph;
        std::vector< std::vector< std::shared_ptr< ImGuiVulkanTexture > > > m_outputTextures;
        size_t m_selectedTexture = 0;

        std::shared_ptr< Camera > m_editorCamera;
        bool m_editorCameraEnabled = false;
        Vector2 m_lastMousePos = Vector2 { 0, 0 };
        Transformation m_cameraRotation = Transformation::Constants::IDENTITY;
        Transformation m_cameraTranslation = Transformation::Constants::IDENTITY;
    };

}

#endif
