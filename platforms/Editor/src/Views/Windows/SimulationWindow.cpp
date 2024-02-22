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

#include "Views/Windows/SimulationWindow.hpp"

#include "Foundation/ImGuiUtils.hpp"
#include "Rendering/FrameGraph/VulkanRenderScene.hpp"
#include "Rendering/VulkanImage.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderTarget.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild::editor;

SimulationWindow::SimulationWindow( void ) noexcept
    : Window( "Simulation" )
{
    // no-op
}

void SimulationWindow::drawContent( void ) noexcept
{
    if ( !m_initialized ) {
        initialize();
    }

    auto device = vulkan::RenderDevice::getInstance();

    ImVec2 renderSize = ImGui::GetContentRegionAvail();
    bool isMinimized = renderSize.x < 1 || renderSize.y < 1;
    m_extent.width = renderSize.x;
    m_extent.height = renderSize.y;

    auto currentFrameIdx = device->getCurrentFrameIndex();
    if ( !m_outputTextures.empty() ) {
        auto tex_id = m_outputTextures[ currentFrameIdx ].front()->getDescriptorSet();
        ImVec2 uv_min = ImVec2( 0.0f, 0.0f );                 // Top-left
        ImVec2 uv_max = ImVec2( 1.0f, 1.0f );                 // Lower-right
        ImVec4 tint_col = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );   // No tint
        ImVec4 border_col = ImVec4( 1.0f, 1.0f, 1.0f, 0.0f ); // 50% opaque white
        const auto aspect = m_simulationExtent.width / m_simulationExtent.height;
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
    } else {
        ImGui::Text( "No scene attachments found" );
    }

    if ( isMinimized ) {
        return;
    }

    auto camera = crimild::Simulation::getInstance()->getMainCamera();
    if ( camera != nullptr ) {
        camera->setAspectRatio( m_extent.width / m_extent.height );
    }

    for ( auto &node : m_framegraph[ currentFrameIdx ] ) {
        node->execute();
    }
}

void SimulationWindow::initialize( void ) noexcept
{
    auto device = vulkan::RenderDevice::getInstance();

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

    const auto extent = VkExtent2D {
        // Render using a squared aspect ratio, so we don't need to
        // resize images when windows does. The end result might be
        // a bit pixelated, but it's faster. And we can always render to
        // a bigger buffer
        .width = 1280,
        .height = 720,
    };

    // Simulation renders at the fixed size. This might change in the future
    const auto N = device->getInFlightFrameCount();
    m_framegraph.resize( N );
    m_outputTextures.resize( N );
    for ( int i = 0; i < N; ++i ) {
        auto renderSceneOutput = createColorImageView( "Scene/Output", extent, VK_FORMAT_R32G32B32A32_SFLOAT );
        auto renderScene = crimild::alloc< vulkan::framegraph::RenderScene >(
            device,
            "Scene",
            nullptr,
            renderSceneOutput
        );

        m_framegraph[ i ] = {
            renderScene,
        };

        m_outputTextures[ i ] = {
            crimild::alloc< ImGuiVulkanTexture >( "Scene", renderSceneOutput ),
        };
    }

    m_initialized = true;
}
