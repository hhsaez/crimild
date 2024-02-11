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

#include "Layout/LayoutManager.hpp"

#include "Coding/FileDecoder.hpp"
#include "Coding/FileEncoder.hpp"
#include "Foundation/ImGuiUtils.hpp"
#include "Views/Windows/LogWindow/LogWindow.hpp"

#include <filesystem>

using namespace crimild::editor;

void Layout::draw( void ) noexcept
{
    ImGui::DockSpaceOverViewport( ImGui::GetMainViewport() );

    std::cout << "Getting widow info from context" << std::endl;

    auto ctx = ImGui::GetCurrentContext();
    if ( ctx != nullptr ) {
        for ( int i = 0; i < ctx->Windows.size(); ++i ) {
            const auto window = ctx->Windows[ i ];
            if ( window != nullptr ) {
                std::cout << i << ": " << window->Name << "\n";
            } else {
                std::cout << i << ": unknown\n";
            }
        }
    } else {
        std::cout << "Context is Null" << std::endl;
    }

    std::cout << "DONE Getting widow info from context" << std::endl;

    for ( auto &view : m_views ) {
        if ( view->isActive() ) {
            view->draw();
        }
    }
}

void Layout::makeCurrent( void ) noexcept
{
    // Disables imgui.ini file since we're going to save ImGui state to our own Layout structure
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;

    if ( !m_imGuiState.empty() ) {
        ImGui::LoadIniSettingsFromMemory( m_imGuiState.c_str() );
    }
}

void Layout::encode( coding::Encoder &encoder ) noexcept
{
    Codable::encode( encoder );

    std::string imGuiState = ImGui::SaveIniSettingsToMemory();
    encoder.encode( "imGuiState", imGuiState );

    ImGuiIO &io = ImGui::GetIO();
    ImVec2 displaySize = io.DisplaySize;
    m_extent.width = displaySize.x;
    m_extent.height = displaySize.y;
    encoder.encode( "extent", m_extent );
}

void Layout::decode( coding::Decoder &decoder ) noexcept
{
    Codable::decode( decoder );

    decoder.decode( "imGuiState", m_imGuiState );
    decoder.decode( "extent", m_extent );

    m_views.clear();
    addView( std::make_shared< LogWindow >() );
}

LayoutManager::LayoutManager( void ) noexcept
{
    const auto currentPath = std::filesystem::current_path();
    const auto layoutFilePath = currentPath / "layout.crimild";

    if ( std::filesystem::exists( layoutFilePath ) ) {
        coding::FileDecoder decoder;
        if ( decoder.read( layoutFilePath ) && decoder.getObjectCount() > 0 ) {
            m_layout = decoder.getObjectAt< Layout >( 0 );
        }
    }

    if ( m_layout == nullptr ) {
        m_layout = std::make_unique< Layout >();
    }
}

LayoutManager::~LayoutManager( void ) noexcept
{
    const auto currentPath = std::filesystem::current_path();
    const auto layoutFilePath = currentPath / "layout.crimild";

    if ( m_layout != nullptr ) {
        coding::FileEncoder encoder;
        encoder.encode( m_layout );
        encoder.write( layoutFilePath );
    }
}
