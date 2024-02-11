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
#include "Views/Menus/MainMenu/MainMenu.hpp"
#include "Views/Windows/FileSystemWindow/FileSystemWindow.hpp"
#include "Views/Windows/InspectorWindow/InspectorWindow.hpp"
#include "Views/Windows/LogWindow/LogWindow.hpp"
#include "Views/Windows/Scene3DWindow/Scene3DWindow.hpp"
#include "Views/Windows/SceneWindow/SceneWindow.hpp"
#include "Views/Windows/SimulationWindow/SimulationWindow.hpp"

#include <filesystem>

using namespace crimild::editor;

const std::string DEFAULT_LAYOUT = R"(
[Window][DockSpaceViewport_11111111]
Pos=0,0
Size=1024,768
Collapsed=0

[Window][FileSystem##crimild::editor::FileSystemWindow]
Pos=0,386
Size=299,382
Collapsed=0
DockId=0x00000009,0

[Window][Inspector##crimild::editor::InspectorWindow]
Pos=724,0
Size=300,768
Collapsed=0
DockId=0x00000007,0

[Window][Log##crimild::editor::LogWindow]
Pos=301,386
Size=421,382
Collapsed=0
DockId=0x0000000A,0

[Window][Scene3D##crimild::editor::Scene3DWindow]
Pos=301,0
Size=211,384
Collapsed=0
DockId=0x00000005,0

[Window][Scene##crimild::editor::SceneWindow]
Pos=0,0
Size=299,384
Collapsed=0
DockId=0x00000008,0

[Window][Simulation##crimild::editor::SimulationWindow]
Pos=514,0
Size=208,384
Collapsed=0
DockId=0x00000006,0

[Window][Debug##Default]
Pos=60,60
Size=400,400
Collapsed=0

[Docking][Data]
DockSpace         ID=0x8B93E3BD Window=0xA787BDB4 Pos=1192,321 Size=1024,768 Split=X
  DockNode        ID=0x00000001 Parent=0x8B93E3BD SizeRef=722,768 Split=X
    DockNode      ID=0x00000002 Parent=0x00000001 SizeRef=299,768 Split=Y Selected=0x634F299B
      DockNode    ID=0x00000008 Parent=0x00000002 SizeRef=299,384 Selected=0x634F299B
      DockNode    ID=0x00000009 Parent=0x00000002 SizeRef=299,382 Selected=0x7651EB40
    DockNode      ID=0x00000003 Parent=0x00000001 SizeRef=0,0 Split=Y
      DockNode    ID=0x00000004 Parent=0x00000003 SizeRef=268,384 Split=X Selected=0xB0558315
        DockNode  ID=0x00000005 Parent=0x00000004 SizeRef=211,384 CentralNode=1 Selected=0xB0558315
        DockNode  ID=0x00000006 Parent=0x00000004 SizeRef=208,384 Selected=0x74E6E789
      DockNode    ID=0x0000000A Parent=0x00000003 SizeRef=268,382 Selected=0x71F254ED
  DockNode        ID=0x00000007 Parent=0x8B93E3BD SizeRef=300,768 Selected=0xFC755B05
)";

Layout::Layout( std::string_view name, std::string_view imGuiLayout ) noexcept
    : Named( name ),
      m_imGuiLayout( imGuiLayout )
{
    addView( std::make_shared< MainMenu >() );
    addView( std::make_shared< FileSystemWindow >() );
    addView( std::make_shared< InspectorWindow >() );
    addView( std::make_shared< LogWindow >() );
    addView( std::make_shared< Scene3DWindow >() );
    addView( std::make_shared< SceneWindow >() );
    addView( std::make_shared< SimulationWindow >() );
}

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
    encoder.encode( "extent", m_extent );

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
    decoder.decode( "extent", m_extent );

    Array< std::shared_ptr< View > > views;
    decoder.decode( "views", views );
    views.each( [ this ]( auto view ) {
        addView( view );
    } );
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
        m_layout = std::make_unique< Layout >( "Default", DEFAULT_LAYOUT );
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
