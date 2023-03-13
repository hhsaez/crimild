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

#include "Panels/SimulationPanel.hpp"

#include "Foundation/ImGuiUtils.hpp"
#include "Rendering/FrameGraph/VulkanRenderScene.hpp"
#include "Rendering/VulkanImageView.hpp"
#include "Rendering/VulkanRenderDevice.hpp"
#include "Rendering/VulkanRenderTarget.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild::editor::panels;

Simulation::Simulation( crimild::vulkan::RenderDevice *device ) noexcept
    : WithRenderDevice( device )
{
    // Simulation renders at the fixed size. This might change in the future
    // m_scenePass.handle( Event { .type = Event::Type::WINDOW_RESIZE, .extent = m_simulationExtent } );

    const auto N = getRenderDevice()->getInFlightFrameCount();
    m_framegraphs.resize( N );
    m_outputTextures.resize( N );
    for ( int i = 0; i < N; ++i ) {
        auto framegraph = crimild::alloc< vulkan::framegraph::RenderScene >(
            device,
            VkExtent2D {
                .width = uint32_t( m_simulationExtent.width ),
                .height = uint32_t( m_simulationExtent.height ),
            }
        );
        m_framegraphs[ i ] = framegraph;
        m_outputTextures[ i ] = crimild::alloc< ImGuiVulkanTexture >(
            framegraph->getOutput()->getImageView(),
            framegraph->getOutput()->getSampler()
        );
    }
}

Simulation::~Simulation( void ) noexcept
{
    getRenderDevice()->flush();
}

crimild::Event Simulation::handle( const crimild::Event &e ) noexcept
{
    // return m_scenePass.handle( e );
    return e;
}

void Simulation::onRender( void ) noexcept
{
    // static auto debouncedResize = concurrency::debounce(
    //     [ this ]( Event e ) {
    //         m_descriptorSets.clear();
    //     },
    //     500
    // );

    ImVec2 renderSize = ImGui::GetContentRegionAvail();
    bool isMinimized = renderSize.x < 1 || renderSize.y < 1;
    m_extent.width = renderSize.x;
    m_extent.height = renderSize.y;
    // if ( !isMinimized ) {
    //     if ( m_extent.width != renderSize.x || m_extent.height != renderSize.y ) {
    //         m_extent.width = renderSize.x;
    //         m_extent.height = renderSize.y;
    //         debouncedResize(
    //             Event {
    //                 .type = Event::Type::WINDOW_RESIZE,
    //                 .extent = m_extent,
    //             }
    //         );
    //     }
    // }

    auto currentFrameIdx = getRenderDevice()->getCurrentFrameIndex();

    // const auto att = m_scenePass.getColorAttachment();
    // if ( !att->descriptorSets.empty() ) {
    // if ( !m_descriptorSets.contains( att ) ) {
    //     auto ds = std::vector< VkDescriptorSet >( m_scenePass.getRenderDevice()->getInFlightFrameCount() );
    //     for ( int i = 0; i < ds.size(); ++i ) {
    //         ds[ i ] = ImGui_ImplVulkan_AddTexture(
    //             att->sampler,
    //             ( VkImageView ) *att->imageViews[ currentFrameIdx ].get(),
    //             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    //         );
    //     }
    //     m_descriptorSets[ att ] = ds;
    // }
    // auto tex_id = m_descriptorSets.at( att )[ currentFrameIdx ];

    if ( !m_outputTextures.empty() ) {
        auto tex_id = m_outputTextures[ currentFrameIdx ]->getDescriptorSet();
        ImVec2 uv_min = ImVec2( 0.0f, 0.0f );                 // Top-left
        ImVec2 uv_max = ImVec2( 1.0f, 1.0f );                 // Lower-right
        ImVec4 tint_col = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );   // No tint
        ImVec4 border_col = ImVec4( 1.0f, 1.0f, 1.0f, 0.0f ); // 50% opaque white
        const auto aspect = m_simulationExtent.width / m_simulationExtent.height;
        auto available = ImGui::GetContentRegionAvail();
        auto imageSize = available;
        if ( imageSize.x > aspect * imageSize.y ) {
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
    // m_scenePass.render( crimild::Simulation::getInstance()->getScene(), camera );
    m_framegraphs[ currentFrameIdx ]->render(
        crimild::Simulation::getInstance()->getScene(),
        camera
    );
    
    getRenderDevice()->flush();
}
