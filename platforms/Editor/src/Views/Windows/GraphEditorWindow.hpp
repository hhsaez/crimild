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

#ifndef CRIMILD_EDITOR_VIEWS_WINDOWS_GRAPH_EDITOR_
#define CRIMILD_EDITOR_VIEWS_WINDOWS_GRAPH_EDITOR_

#include "Foundation/ImGuiUtils.hpp"
#include "Views/Windows/Window.hpp"

#include <imgui_node_editor.h>

namespace crimild::editor {

    enum class PinType {
        Flow,
        Bool,
        Int,
        Float,
        String,
        Object,
        Function,
        Delegate,
    };

    enum class PinKind {
        Output,
        Input,
    };

    enum class NodeType {
        Blueprint,
        Simple,
        Tree,
        Comment,
        Houdini,
    };

    struct Node;

    struct Pin {
        ax::NodeEditor::PinId id;
        Node *node;
        std::string name;
        ax::NodeEditor::PinType type;
        ax::NodeEditor::PinKind kind;

        Pin( ax::NodeEditor::PinId id, std::string_view name, PinType type )
            : id( id ), node( nullptr ), name( name ), type( type ), kind( PinKind::Input )
        {
            // no-op
        }
    };

    struct Node {
        ax::NodeEditor::NodeId id;
        std::string name;
        std::vector< Pin > inputs;
        std::vector< Pin > outputs;
        ImColor color;
        NodeType type;
        ImVec2 size;

        std::string state;
        std::string savedState;

        Node( ax::NodeEditor::NodeId id, std::string_view name, ImColor color = ImColor( 255, 255, 255 ) )
            : id( id ), name( name ), color( color ), type( NodeType::Blueprint ), size( 0, 0 )
        {
            // no-op
        }
    };

    struct Link {
        ax::NodeEditor::LinkId id;
        ax::NodeEditor::PinId startPinId;
        ax::NodeEditor::PinId endPinId;
        ImColor color;

        Link( ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId, ImColor color = ImColor( 255, 255, 255 ) )
            : id( id ), startPinId( startPinId ), endPinId( endPinId ), color( color )
        {
            // no-op
        }
    };

    struct NodeIdLess {
        bool operator()( const ax::NodeEditor::NodeId &lhs, const ax::NodeEditor::NodeId &rhs ) const
        {
            return lhs.AsPointer() < rhs.AsPointer();
        }
    };

    // Holds basic information about connections between
    // pins. Note that connection (aka, link) has its own
    // ID. This is useful later with dealing with selections,
    // deletion and other operations.
    struct LinkInfo {
        ax::NodeEditor::LinkId id;
        ax::NodeEditor::PinId inputPinId;
        ax::NodeEditor::PinId outputPinId;
    };

    class GraphEditorWindow : public Window {
        CRIMILD_IMPLEMENT_RTTI( crimild::editor::GraphEditorWindow )

    public:
        GraphEditorWindow( void ) noexcept;
        ~GraphEditorWindow( void ) noexcept;

        void drawContent( void ) noexcept final;

    private:
        void updateTouch( void ) noexcept;
        void showLeftPanel( float panelWidth ) noexcept;

    private:
        // Editor context
        // Required to trace editor state.
        ax::NodeEditor::EditorContext *m_context = nullptr;

        // Flag set for first frame only
        // This is required for some actions that need to be executed only once
        // bool m_firstFrame = true;

        // List of live links
        // It is dynamic unless you want to create a read-only view over nodes
        // ImVector< LinkInfo > m_links;

        // Counter to help generate link ids.
        // In a real application this will probably based on pointer to user
        // data structures
        // int m_nextLinkId = 100;

        int m_nextId = 1;
        const int m_pinIconSize = 24;
        std::vector< Node > m_nodes;
        std::vector< Link > m_links;
        ImTextureID m_headerBackground = nullptr;
        ImTextureID m_saveIcon = nullptr;
        ImTextureID m_restoreIcon = nullptr;
        const float m_touchTime = 1.0f;
        std::map< ax::NodeEditor::NodeId, float NodeIdLess > m_nodeTouchTime;
        bool m_showOrdinals = false;
    };
}

#endif
