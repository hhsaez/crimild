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

#include "Simulation/Systems/ImGUISystem.hpp"

#include "Rendering/Compositions/ImGUIComposition.hpp"
#include "Rendering/Compositions/OverlayComposition.hpp"
#include "Rendering/Compositions/PresentComposition.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Operations/ImGUIOperations.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/Texture.hpp"
#include "Simulation/Input.hpp"
#include "Simulation/Simulation.hpp"
#include "Simulation/Systems/RenderSystem.hpp"
#include "imgui.h"

using namespace crimild;
using namespace crimild::imgui;
using namespace crimild::framegraph::imgui;

void ImGUISystem::start( void ) noexcept
{
    System::start();

    CRIMILD_LOG_TRACE( "Starting ImGUI System" );

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    auto &io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    updateDisplaySize();

    registerMessageHandler< messaging::KeyPressed >( [ this ]( messaging::KeyPressed const &msg ) {
        // int key = msg.key;
        // self->_keys[ key ] = true;
    } );

    registerMessageHandler< messaging::KeyReleased >( [ this ]( messaging::KeyReleased const &msg ) {
        // int key = msg.key;
        // self->_keys[ key ] = false;
    } );

    registerMessageHandler< messaging::MouseButtonDown >( [ this ]( messaging::MouseButtonDown const &msg ) {
        int button = msg.button;
        auto &io = ImGui::GetIO();
        io.MouseDown[ button ] = true;
    } );

    registerMessageHandler< messaging::MouseButtonUp >( [ this ]( messaging::MouseButtonUp const &msg ) {
        int button = msg.button;
        auto &io = ImGui::GetIO();
        io.MouseDown[ button ] = false;
    } );

    registerMessageHandler< messaging::MouseMotion >( [ this ]( messaging::MouseMotion const &msg ) {
        auto &io = ImGui::GetIO();
        io.MousePos = ImVec2( msg.x, msg.y );
        // Vector2f pos( msg.x, msg.y );
        // self->_mouseDelta = pos - self->_mousePos;
        // self->_mousePos = pos;

        // Vector2f npos( msg.nx, msg.ny );
        // self->_normalizedMouseDelta = npos - self->_normalizedMousePos;
        // self->_normalizedMousePos = npos;
    } );

    registerMessageHandler< messaging::MouseScroll >( [ this ]( messaging::MouseScroll const &msg ) {
        // _mouseScrollDelta = Vector2f( msg.dx, msg.dy );
    } );

    setFrameCallback(
        [] {
            auto sim = Simulation::getInstance();
            auto settings = sim->getSettings();
            auto showDebug = settings->get< Bool >( "debug.show", false );
            if ( !showDebug ) {
                return;
            }

            auto frameTime = sim->getSimulationClock().getDeltaTime();
            ImGui::Begin( "Stats" );
            ImGui::Text( "Frame Time: %.2f ms", 1000.0f * frameTime );
            ImGui::Text( "FPS: %d", frameTime > 0 ? int( 1.0 / frameTime ) : 0 );
            ImGui::End();
        } );
}

void ImGUISystem::lateStart( void ) noexcept
{
    auto frameGraph = RenderSystem::getInstance()->getFrameGraph();

    if ( frameGraph == nullptr ) {
        // This should not happen, since we always have something to render
        frameGraph = renderUI();
    } else {
        // We want to render the ImGUI layer on top of whatever we rendered before
        frameGraph = overlayUI( renderUI(), frameGraph );
    }

    RenderSystem::getInstance()->setFrameGraph( frameGraph );
}

void ImGUISystem::onPreRender( void ) noexcept
{
    auto &io = ImGui::GetIO();

    auto clock = Simulation::getInstance()->getSimulationClock();
    io.DeltaTime = Numericf::max( 1.0f / 60.0f, clock.getDeltaTime() );

    if ( !io.Fonts->IsBuilt() ) {
        CRIMILD_LOG_ERROR( "Font atlas is not built!" );
        return;
    }

    updateDisplaySize();

    ImGui::NewFrame();

    if ( m_frameCallback != nullptr ) {
        m_frameCallback();
    }

    ImGui::Render();
}

void ImGUISystem::onTerminate( void ) noexcept
{
    System::onTerminate();

    ImGui::DestroyContext();
}

void ImGUISystem::updateDisplaySize( void ) noexcept
{
    auto &io = ImGui::GetIO();
    auto width = Simulation::getInstance()->getSettings()->get< float >( "video.width", 0 );
    auto height = Simulation::getInstance()->getSettings()->get< float >( "video.height", 1 );
    auto scale = 1.0f;
    io.DisplaySize = ImVec2( scale * width, scale * height );
    io.DisplayFramebufferScale = ImVec2( 1.0f, 1.0f );
}
