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

#include "Layout/Layout.hpp"

#include "Foundation/ImGuiUtils.hpp"
#include "Views/View.hpp"

#include <Crimild_Coding.hpp>

using namespace crimild::editor;

Layout::Layout( std::string_view name, std::string_view imGuiLayout ) noexcept
    : Named( name ),
      m_imGuiLayout( imGuiLayout )
{
}

bool Layout::hasViewWithTitle( std::string_view title ) const noexcept
{
    for ( const auto &view : m_views ) {
        if ( view->getName() == title ) {
            return true;
        }
    }
    return false;
}

void Layout::addView( std::shared_ptr< View > const &view ) noexcept
{
    m_views.push_back( view );
    view->setLayout( std::static_pointer_cast< Layout >( shared_from_this() ) );
}

void Layout::draw( void ) noexcept
{
    ImGui::DockSpaceOverViewport( 0, ImGui::GetMainViewport() );

    // std::cout << "Getting widow info from context" << std::endl;

    // auto ctx = ImGui::GetCurrentContext();
    // if ( ctx != nullptr ) {
    //     for ( int i = 0; i < ctx->Windows.size(); ++i ) {
    //         const auto window = ctx->Windows[ i ];
    //         if ( window != nullptr ) {
    //             std::cout << i << ": " << window->Name << "\n";
    //         } else {
    //             std::cout << i << ": unknown\n";
    //         }
    //     }
    // } else {
    //     std::cout << "Context is Null" << std::endl;
    // }

    // std::cout << "DONE Getting widow info from context" << std::endl;

    auto views = m_views;
    bool shouldRefreshConfig = false;
    for ( auto &view : views ) {
        if ( view->isOpen() ) {
            view->draw();
        } else {
            const auto it = std::find( std::begin( m_views ), std::end( m_views ), view );
            if ( it != std::end( m_views ) ) {
                m_views.erase( it );
                shouldRefreshConfig = true;
            }
        }
    }

    if ( shouldRefreshConfig ) {
        // Marks ImGui settings as dirty whenever the panel's open state changes
        // This forces ImGui settings to be saved to disk as soon as possible.
        ImGui::MarkIniSettingsDirty();
    }
}

void Layout::makeCurrent( void ) noexcept
{
    // Disables imgui.ini file since we're going to save ImGui state to our own Layout structure
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;

    if ( !m_imGuiLayout.empty() ) {
        ImGui::LoadIniSettingsFromMemory( m_imGuiLayout.c_str() );
    }
}

void Layout::encode( coding::Encoder &encoder ) noexcept
{
    Codable::encode( encoder );

    encoder.encode( "name", getName() );

    m_imGuiLayout = ImGui::SaveIniSettingsToMemory();
    encoder.encode( "imGuiLayout", m_imGuiLayout );
    std::cout << "ImGui State:\n"
              << m_imGuiLayout << std::endl;

    ImGuiIO &io = ImGui::GetIO();
    ImVec2 displaySize = io.DisplaySize;
    m_extent.width = displaySize.x;
    m_extent.height = displaySize.y;
    encoder.encode( "extent.width", m_extent.width );
    encoder.encode( "extent.height", m_extent.height );
    encoder.encodeEnum( "extent.scaling_mode", m_extent.scalingMode );

    Array< std::shared_ptr< View > > views;
    for ( auto &view : m_views ) {
        views.add( view );
    }
    encoder.encode( "views", views );
}

void Layout::decode( coding::Decoder &decoder ) noexcept
{
    Codable::decode( decoder );

    decoder.decode( "name", getName() );

    decoder.decode( "imGuiLayout", m_imGuiLayout );
    decoder.decode( "extent.width", m_extent.width );
    decoder.decode( "extent.height", m_extent.height );
    decoder.decodeEnum( "extent.scaling_mode", m_extent.scalingMode );

    Array< std::shared_ptr< View > > views;
    decoder.decode( "views", views );
    views.each( [ this ]( auto view ) {
        addView( view );
    } );
}
