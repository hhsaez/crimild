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

#include "Foundation/ImGuiUtils.hpp"
#include "Layout/Layout.hpp"
#include "Views/Menus/MainMenu/MainMenu.hpp"
#include "Views/Windows/FileSystemWindow.hpp"
#include "Views/Windows/InspectorWindow.hpp"
#include "Views/Windows/LogWindow.hpp"
#include "Views/Windows/Scene3DWindow.hpp"
#include "Views/Windows/SceneWindow.hpp"
#include "Views/Windows/SimulationWindow.hpp"
#include "Views/Workspaces/WorkspaceManager.hpp"

#include <Crimild_Coding.hpp>
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

LayoutManager::LayoutManager( void ) noexcept
{
   const auto currentPath = std::filesystem::current_path();
   const auto layoutFilePath = currentPath / "layout.crimild";

   if ( std::filesystem::exists( layoutFilePath ) ) {
      coding::FileDecoder decoder;
      if ( decoder.read( layoutFilePath ) && decoder.getObjectCount() > 0 ) {
         // m_layout = decoder.getObjectAt< Layout >( 0 );
      }
   }

   if ( m_layout == nullptr ) {
      m_layout = std::make_unique< Layout >( "Default", DEFAULT_LAYOUT );
      m_layout->addView( crimild::alloc< MainMenu >() );
      m_layout->addView( crimild::alloc< WorkspaceManager >() );
      //  m_layout->addView( std::make_shared< FileSystemWindow >() );
      //  m_layout->addView( std::make_shared< InspectorWindow >() );
      //  m_layout->addView( std::make_shared< LogWindow >() );
      //  m_layout->addView( std::make_shared< Scene3DWindow >() );
      //  m_layout->addView( std::make_shared< SceneWindow >() );
      //  m_layout->addView( std::make_shared< SimulationWindow >() );
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

void LayoutManager::init( void )
{
   if ( m_layout != nullptr ) {
      m_layout->makeCurrent();
   }
}

void LayoutManager::render( void )
{
   if ( m_layout != nullptr ) {
      m_layout->draw();
   }
}
