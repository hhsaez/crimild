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

#include <imgui.h>
#include <imgui_internal.h>

using namespace crimild;
using namespace crimild::editor;

layout::Layout::Layout( std::string_view name ) noexcept
    : NamedObject( std::string( name ) )
{
    
}

layout::Layout::Layout( Direction direction, float fraction ) noexcept
    : m_direction( direction ),
      m_fraction( fraction )
{
    
}

layout::Layout::Layout( Direction direction, uint32_t pixels ) noexcept
    : m_direction( direction ),
      m_pixels( pixels )
{
    
}

void layout::Layout::setFirst( std::shared_ptr< Layout > const &first ) noexcept
{
    if ( m_first != nullptr ) {
        m_first->setParent( nullptr );
    }
    m_first = first;
    if ( m_first != nullptr ) {
        m_first->setParent( this );
    }
    
    // TODO: if child is null and direction != none, maybe replace this node?
    onLayoutChanged();
}

void layout::Layout::setSecond( std::shared_ptr< Layout > const &second ) noexcept
{
    if ( m_second != nullptr ) {
        m_second->setParent( nullptr );
    }
    m_second = second;
    if ( m_second != nullptr ) {
        m_second->setParent( this );
    }
    // TODO: if child is null and direction != none, maybe replace this node?
    onLayoutChanged();
}

Event layout::Layout::handle( const Event &e ) noexcept
{
    auto ret = e;
    if ( m_first ) {
        ret = m_first->handle( ret );
    }
    
    if ( m_second != nullptr ) {
        ret = m_second->handle( ret );
    }
    return ret;
}

void layout::Layout::build( ImGuiID id ) noexcept
{
    m_id = id;
    
    auto ctx = ImGui::GetCurrentContext();
    auto node = ImGui::DockContextFindNodeByID( ctx, m_id );
    m_size = node->Size;
    m_pos = node->Pos;
    
    ImGuiID firstID = id;
    ImGuiID secondID = id;
    
    if ( m_direction != Direction::NONE ) {
        if ( m_direction == Direction::LEFT || m_direction == Direction::RIGHT ) {
            if ( m_pixels > 0 ) {
                m_fraction = float( m_pixels ) / m_size.x;
            }
        } else {
            if ( m_pixels > 0 ) {
                m_fraction = float( m_pixels ) / m_size.y;
            }
        }
        ImGui::DockBuilderSplitNode( getID(), int( m_direction ), m_fraction, &firstID, &secondID );
    }
    
    if ( m_first != nullptr ) {
        m_first->build( firstID );
    }
    
    if ( m_second != nullptr ) {
        m_second->build( secondID );
    }
}

void layout::Layout::render( void ) noexcept
{
    if ( m_first != nullptr ) {
        m_first->render();
    }
    
    if ( m_second != nullptr ) {
        m_second->render();
    }
}

void layout::Layout::removeFromParent( void ) noexcept
{
    concurrency::sync_frame(
        [ & ] {
            if ( auto parent = getParent() ) {
                if ( parent->getFirst().get() == this ) {
                    parent->setFirst( nullptr );
                } else if ( parent->getSecond().get() == this ) {
                    parent->setSecond( nullptr );
                }
            }
        }
    );
}

void layout::Layout::onLayoutChanged( void ) noexcept
{
    if ( auto parent = getParent() ) {
        parent->onLayoutChanged();
    }
}

Event layout::Dockspace::handle( const Event &e ) noexcept
{
    if ( e.type == Event::Type::WINDOW_RESIZE ) {
        m_extent = e.extent;
        m_reset = true;
    }
    
    return Layout::handle( e );
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
        
        build( dockspaceId );

        ImGui::DockBuilderFinish( dockspaceId );

        m_reset = false;
    }

    ImGui::End();
    
    Layout::render();
}

//Event layout::Tab::handle( const Event &e ) noexcept
//{
//    for ( auto &child : m_children ) {
//        child->handle( e );
//    }
//    return Node::handle( e );
//}
//
//void layout::Tab::build( ImGuiID parentId ) noexcept
//{
//    for ( auto &child : m_children ) {
//        child->build( parentId );
//    }
//}
//
//void layout::Tab::render( void ) noexcept
//{
//    for ( auto &child : m_children ) {
//        child->render();
//    }
//}
//
//void layout::Tab::removeChild( Node *child ) noexcept
//{
//    auto it = std::find_if( m_children.begin(), m_children.end(), [&]( auto &other ) { return other.get() == child; } );
//    if ( it != m_children.end() ) {
//        m_children.erase( it );
//        child->setParent( nullptr );
//    }
//    if ( m_children.size() == 1 ) {
//        getParent()->replaceChild( this, m_children.front() );
//    } else {
//        onLayoutChanged();
//    }
//}
//
//void layout::Tab::replaceChild( Node *child, std::shared_ptr< Node > const newChild ) noexcept
//{
//    auto it = std::find_if( m_children.begin(), m_children.end(), [&]( auto &other ) { return other.get() == child; } );
//    if ( it != m_children.end() ) {
//        m_children.erase( it );
//        child->setParent( nullptr );
//    }
//    addChild( newChild );
//    if ( m_children.size() == 1 ) {
//        getParent()->replaceChild( this, m_children.front() );
//    } else {
//        onLayoutChanged();
//    }
//}

layout::Panel::Panel( std::string_view name ) noexcept
    : Layout( name )
{
    
}

void layout::Panel::build( ImGuiID id ) noexcept
{
    Layout::build( id );
    
    ImGui::DockBuilderDockWindow( getUniqueName().c_str(), getID() );
}
