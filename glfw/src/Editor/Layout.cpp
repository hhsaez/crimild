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

#include <imgui.h>
#include <imgui_internal.h>

using namespace crimild;
using namespace crimild::editor;

Event layout::Node::handle( const Event &e ) noexcept
{
    return e;
}

Event layout::Dockspace::handle( const Event &e ) noexcept
{
    if ( e.type == Event::Type::WINDOW_RESIZE ) {
        m_extent = e.extent;
        m_reset = true;
    }
    
    return root->handle( Node::handle( e ) );
}

void layout::Dockspace::render( void ) noexcept
{
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
    ImGui::Begin( "DockSpace", &p_open, window_flags );
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
            
        auto dockspaceId = ImGui::GetID( "dockspace" );
        ImGui::DockSpace( dockspaceId, ImVec2( 0.0f, 0.0f ), dockspace_flags );
        
        if ( m_reset ) {
            ImGui::DockBuilderRemoveNode( dockspaceId ); // clear previous layout
            
            ImGui::DockBuilderAddNode( dockspaceId, dockspace_flags | ImGuiDockNodeFlags_DockSpace );
            ImGui::DockBuilderSetNodeSize( dockspaceId, size );
            
            root->build( dockspaceId );
            
            ImGui::DockBuilderFinish( dockspaceId );
            
            m_reset = false;
        }
        
        ImGui::End();
    
    root->render();
}

void layout::Dockspace::build( ImGuiID ) noexcept
{
    // do nothing
}

Event layout::VerticalSplit::handle( const Event &e ) noexcept
{
    return right->handle( left->handle( Node::handle( e ) ) );
}

void layout::VerticalSplit::build( ImGuiID parentId ) noexcept
{
    ImGuiID leftId, rightId;
    ImGui::DockBuilderSplitNode( parentId, ImGuiDir_Left, fraction, &leftId, &rightId );
    
    left->build( leftId );
    right->build( rightId );
}

void layout::VerticalSplit::render( void ) noexcept
{
    left->render();
    right->render();
}

Event layout::HorizontalSplit::handle( const Event &e ) noexcept
{
    return bottom->handle( top->handle( Node::handle( e ) ) );
}

void layout::HorizontalSplit::build( ImGuiID parentId ) noexcept
{
    ImGuiID topId, bottomId;
    ImGui::DockBuilderSplitNode( parentId, ImGuiDir_Up, fraction, &topId, &bottomId );
    
    top->build( topId );
    bottom->build( bottomId );
}

void layout::HorizontalSplit::render( void ) noexcept
{
    top->render();
    bottom->render();
}

Event layout::Panel::handle( const Event &e ) noexcept
{
    return Node::handle( e );
}

void layout::Panel::build( ImGuiID parentId ) noexcept
{
    ImGui::DockBuilderDockWindow( getUniqueName().c_str(), parentId );
}

