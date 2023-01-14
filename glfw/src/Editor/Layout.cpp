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
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Editor/Layout.hpp"

#include "Concurrency/Async.hpp"
#include "Editor/EditorLayer.hpp"
#include "Editor/Panels/BehaviorEditorPanel.hpp"
#include "Editor/Panels/EditorProjectPanel.hpp"
#include "Editor/Panels/NodeInspectorPanel.hpp"
#include "Editor/Panels/SceneHierarchyPanel.hpp"
#include "Editor/Panels/ScenePanel.hpp"
#include "Editor/Panels/SimulationPanel.hpp"
#include "Editor/Panels/ToolbarPanel.hpp"

#include <imgui.h>
#include <imgui_internal.h>

using namespace crimild;
using namespace crimild::editor;

const std::string DEFAULT_LAYOUT = R"(
[Window][LayoutManager]
Pos=0,0
Size=1920,1080
Collapsed=0

[Window][Debug##Default]
Pos=60,60
Size=400,400
Collapsed=0

[Window][Project]
Pos=1109,685
Size=455,395
Collapsed=0
DockId=0x0000000A,0

[Window][Inspector]
Pos=1566,53
Size=354,1027
Collapsed=0
DockId=0x00000005,0

[Window][Scene Hierarchy]
Pos=1109,53
Size=455,630
Collapsed=0
DockId=0x00000009,0

[Window][Behaviors]
Pos=0,682
Size=1107,398
Collapsed=0
DockId=0x00000008,0

[Window][Scene]
Pos=0,53
Size=1107,627
Collapsed=0
DockId=0x00000007,0

[Window][Toolbar]
Pos=0,19
Size=1920,32
Collapsed=0
DockId=0x00000001

[Docking][Data]
DockSpace         ID=0x25045BC6 Window=0x04ECA795 Pos=0,19 Size=1920,1061 Split=Y
  DockNode        ID=0x00000001 Parent=0x25045BC6 SizeRef=1920,32 Selected=0x738351EE
  DockNode        ID=0x00000002 Parent=0x25045BC6 SizeRef=1920,1027 Split=X
    DockNode      ID=0x0000000B Parent=0x00000002 SizeRef=0,0 Split=Y
      DockNode    ID=0x00000007 Parent=0x0000000B SizeRef=1431,627 CentralNode=1 Selected=0xE192E354
      DockNode    ID=0x00000008 Parent=0x0000000B SizeRef=1431,398 Selected=0xD79FB21B
    DockNode      ID=0x00000003 Parent=0x00000002 SizeRef=811,1028 Split=X
      DockNode    ID=0x00000004 Parent=0x00000003 SizeRef=455,1027 Split=Y Selected=0x2E9237F7
        DockNode  ID=0x00000009 Parent=0x00000004 SizeRef=122,630 Selected=0x2E9237F7
        DockNode  ID=0x0000000A Parent=0x00000004 SizeRef=122,395 Selected=0xD04A4B96
      DockNode    ID=0x00000005 Parent=0x00000003 SizeRef=354,1027 Selected=0xE7039252
)";

layout::Layout::Layout( std::string_view name ) noexcept
    : Named( std::string( name ) )
{
}

Event layout::Layout::handle( const Event &e ) noexcept
{
    return e;
}

void layout::Layout::render( void ) noexcept
{
}

layout::Panel::Panel( std::string_view name ) noexcept
    : Layout( name )
{
}

void layout::Panel::removeFromParent( void ) noexcept
{
    concurrency::sync_frame(
        [ & ] {
            if ( auto manager = LayoutManager::getInstance() ) {
                manager->detachPanel( this );
            }
        }
    );
}

layout::LayoutManager::LayoutManager( void ) noexcept
    : layout::Layout( "LayoutManager" )
{
}

layout::LayoutManager::~LayoutManager( void ) noexcept
{
    m_panels.clear();
}

Event layout::LayoutManager::handle( const Event &e ) noexcept
{
    if ( e.type == Event::Type::WINDOW_RESIZE ) {
        m_extent = e.extent;
    }

    Event ret = e;
    for ( auto &panel : m_panels ) {
        ret = panel->handle( ret );
    }
    return ret;
}

void layout::LayoutManager::render( void ) noexcept
{
    if ( !m_loaded ) {
        auto &context = *GImGui;
        for ( ImGuiWindowSettings *settings = context.SettingsWindows.begin(); settings != nullptr; settings = context.SettingsWindows.next_chunk( settings ) ) {
            // TODO: Have a factory of panels?
            if ( strcmp( settings->GetName(), editor::ScenePanel::NAME ) == 0 ) {
                attachPanel( crimild::alloc< editor::ScenePanel >( EditorLayer::getInstance()->getRenderDevice() ) );
            } else if ( strcmp( settings->GetName(), editor::SimulationPanel::NAME ) == 0 ) {
                attachPanel( crimild::alloc< editor::SimulationPanel >( EditorLayer::getInstance()->getRenderDevice() ) );
            } else if ( strcmp( settings->GetName(), editor::SceneHierarchyPanel::NAME ) == 0 ) {
                attachPanel( crimild::alloc< editor::SceneHierarchyPanel >() );
            } else if ( strcmp( settings->GetName(), editor::NodeInspectorPanel::NAME ) == 0 ) {
                attachPanel( crimild::alloc< editor::NodeInspectorPanel >( EditorLayer::getInstance()->getRenderDevice() ) );
            } else if ( strcmp( settings->GetName(), editor::BehaviorEditorPanel::NAME ) == 0 ) {
                attachPanel( crimild::alloc< editor::BehaviorEditorPanel >() );
            } else if ( strcmp( settings->GetName(), editor::ProjectPanel::NAME ) == 0 ) {
                attachPanel( crimild::alloc< editor::ProjectPanel >() );
            } else if ( strcmp( settings->GetName(), editor::ToolbarPanel::NAME ) == 0 ) {
                attachPanel( crimild::alloc< editor::ToolbarPanel >() );
            }
        }
    }

    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if ( dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode ) {
        window_flags |= ImGuiWindowFlags_NoBackground;
    }

    const auto size = ImVec2( m_extent.width, m_extent.height );

    ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
    ImGui::SetNextWindowSize( size );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );

    static bool p_open = true;
    ImGui::Begin( getName().c_str(), &p_open, window_flags );
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    auto dockspaceId = ImGui::GetID( getName().c_str() );
    ImGui::DockSpace( dockspaceId, ImVec2( 0.0f, 0.0f ), dockspace_flags );

    if ( m_reset ) {
        ImGui::DockBuilderRemoveNode( dockspaceId ); // clear previous layout

        ImGui::DockBuilderAddNode( dockspaceId, dockspace_flags | ImGuiDockNodeFlags_DockSpace );
        ImGui::DockBuilderSetNodeSize( dockspaceId, size );

        for ( auto &panel : m_panels ) {
            ImGui::DockBuilderDockWindow( panel->getName().c_str(), dockspaceId );
        }

        ImGui::DockBuilderFinish( dockspaceId );

        m_reset = false;
    }

    ImGui::End();

    // Closing a panel will change the collection, so copy it before rendering
    auto panels = m_panels;
    for ( auto &panel : panels ) {
        if ( m_loaded ) {
            bool wasReopened = m_reopenedWindows.contains( panel->getName() );
            ImGui::SetNextWindowPos( ImVec2( 100, 100 ), wasReopened ? ImGuiCond_Always : ImGuiCond_FirstUseEver );
            ImGui::SetNextWindowSize( ImVec2( 300, 400 ), ImGuiCond_FirstUseEver );
        }
        panel->render();
    }

    m_reopenedWindows.clear();

    for ( auto &closedWindow : m_closedWindows ) {
        ImGui::SetNextWindowPos( ImVec2( 999999, 999999 ) );
        auto flags = ImGuiWindowFlags_NoSavedSettings;
        bool open = false;
        ImGui::Begin( closedWindow.c_str(), &open, flags );
        ImGui::End();
    }

    m_loaded = true;
}

void layout::LayoutManager::attachPanel( std::shared_ptr< Panel > const &panel ) noexcept
{
    if ( m_closedWindows.find( panel->getName() ) != m_closedWindows.end() ) {
        m_closedWindows.erase( panel->getName() );
        m_reopenedWindows.insert( panel->getName() );
    }

    m_panels.push_back( panel );
}

void layout::LayoutManager::detachPanel( Panel *panel ) noexcept
{
    m_closedWindows.insert( panel->getName() );

    auto it = std::find_if(
        m_panels.begin(),
        m_panels.end(),
        [ & ]( auto other ) { return other.get() == panel; }
    );
    if ( it != m_panels.end() ) {
        m_panels.erase( it );
    }

    // Regenerate settings file so this window is no longer included.
    ImGui::ClearIniSettings();
}

void layout::LayoutManager::clear( void ) noexcept
{
    for ( auto panel : m_panels ) {
        m_closedWindows.insert( panel->getName() );
    }

    ImGui::ClearIniSettings();
    m_panels.clear();
    m_reset = true;
}

void layout::LayoutManager::loadDefaultLayout( void ) noexcept
{
    clear();

    concurrency::sync_frame(
        [ this ] {
            ImGui::LoadIniSettingsFromMemory( DEFAULT_LAYOUT.c_str(), DEFAULT_LAYOUT.size() );
            ImGui::MarkIniSettingsDirty();
            m_loaded = false;
            m_reset = false;
        }
    );
}
