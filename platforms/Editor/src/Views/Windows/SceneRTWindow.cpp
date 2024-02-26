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

#include "Views/Windows/SceneRTWindow.hpp"

#include "Rendering/FrameGraph/VulkanRenderSceneRT.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderTarget.hpp"
#include "Simulation/Editor.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;
using namespace crimild::editor;

SceneRTWindow::SceneRTWindow( void ) noexcept
    : Window( "Scene RT" )
{
    // no-op
}

void SceneRTWindow::drawContent( void ) noexcept
{
    if ( m_framegraph == nullptr ) {
        auto device = vulkan::RenderDevice::getInstance();
        m_framegraph = crimild::alloc< vulkan::framegraph::RenderSceneRT >( device, "SceneRT", VkExtent2D { 200, 200 } );
        m_outputTexture = crimild::alloc< ImGuiVulkanTexture >(
            "/SceneRTOutput",
            m_framegraph->getOutput()->getImageView(),
            m_framegraph->getOutput()->getSampler()
        );
    }

    ImVec2 renderSize = ImGui::GetContentRegionAvail();
    bool isMinimized = renderSize.x < 1 || renderSize.y < 1;

    auto tex_id = m_outputTexture->getDescriptorSet();
    ImVec2 uv_min = ImVec2( 0.0f, 0.0f );
    ImVec2 uv_max = ImVec2( 1.0f, 1.0f );
    ImVec4 tint_col = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
    ImVec4 border_col = ImVec4( 1.0f, 1.0f, 1.0f, 0.0f );
    const auto aspect = m_framegraph->getExtent().width / m_framegraph->getExtent().height;
    auto available = ImGui::GetContentRegionAvail();
    auto imageSize = available;
    if ( imageSize.x >= aspect * imageSize.y ) {
        imageSize.x = aspect * imageSize.y;
        ImGui::SetCursorPosX( 0.5f * ( available.x - imageSize.x ) );
    } else {
        imageSize.y = imageSize.x / aspect;
        ImGui::SetCursorPosY( 0.5f * ( available.y - imageSize.y ) );
    }
    ImGui::Image( tex_id, imageSize, uv_min, uv_max, tint_col, border_col );

    ImGui::SetCursorPos( ImVec2( 20, 40 ) );
    if ( ImGui::BeginChild( "Settings", ImVec2( 200, 200 ) ) ) {
        ImGui::Text( "Sample Count: %d", ( int ) m_framegraph->getSampleCount() );
        ImGui::Text( "Sample Progress: %.2f%%", m_framegraph->getProgress() );
        auto backgroundColor = m_framegraph->getBackgroundColor();
        ImGui::ColorEdit3( "Background", get_ptr( backgroundColor ) );
        m_framegraph->setBackgroundColor( backgroundColor );
        if ( m_framegraph->getState() == vulkan::framegraph::RenderSceneRT::State::RUNNING ) {
            if ( ImGui::Button( "Pause" ) ) {
                m_framegraph->setState( vulkan::framegraph::RenderSceneRT::State::PAUSED );
            }
            ImGui::SameLine();
        }
        if ( ImGui::Button( "Restart" ) ) {
            m_framegraph->reset();
        }
    }
    ImGui::EndChild();

    if ( isMinimized ) {
        return;
    }

    if ( auto scene = Simulation::getInstance()->getScene() ) {
        if ( auto camera = Simulation::getInstance()->getMainCamera() ) {
            camera->setAspectRatio( m_framegraph->getExtent().width / m_framegraph->getExtent().height );
            m_framegraph->render( scene, camera );
        }
    }
}
