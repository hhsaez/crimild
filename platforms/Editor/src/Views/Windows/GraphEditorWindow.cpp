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

#include "Views/Windows/GraphEditorWindow.hpp"

using namespace crimild;
using namespace crimild::editor;

namespace NodeEditor = ax::NodeEditor;

namespace ImGui::ex {

    void BeginColumn( void ) noexcept
    {
        ImGui::BeginGroup();
    }

    void NextColumn( void ) noexcept
    {
        ImGui::EndGroup();
        ImGui::SameLine();
        ImGui::BeginGroup();
    }

    void EndColumn( void ) noexcept
    {
        ImGui::EndGroup();
    }

}

GraphEditorWindow::GraphEditorWindow( void ) noexcept
    : Window( "Graph Editor" )
{
    NodeEditor::Config config;
    config.SettingsFile = "./NodeEditor.json";
    m_context = ax::NodeEditor::CreateEditor( &config );
}

GraphEditorWindow::~GraphEditorWindow( void ) noexcept
{
    NodeEditor::DestroyEditor( m_context );
}

void GraphEditorWindow::drawContent( void ) noexcept
{
    auto &io = ImGui::GetIO();

    ImGui::Text( "FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0 );

    ImGui::Separator();

    NodeEditor::SetCurrentEditor( m_context );

    NodeEditor::Begin( "My Editor", ImVec2( 0.0f, 0.0f ) );
    int uniqueId = 1;

    NodeEditor::BeginNode( uniqueId++ );
    ImGui::Text( "Node Test" );
    NodeEditor::BeginPin( uniqueId++, NodeEditor::PinKind::Input );
    ImGui::Text( "-> In" );
    NodeEditor::EndPin();
    ImGui::SameLine();
    NodeEditor::BeginPin( uniqueId++, NodeEditor::PinKind::Output );
    ImGui::Text( "Out ->" );
    NodeEditor::EndPin();
    NodeEditor::EndNode();

    // Node A
    NodeEditor::NodeId nodeAId = uniqueId++;
    NodeEditor::PinId nodeAInputPinId = uniqueId++;
    NodeEditor::PinId nodeAOutputPinId = uniqueId++;
    if ( m_firstFrame ) {
        NodeEditor::SetNodePosition( nodeAId, ImVec2( 10, 10 ) );
    }
    NodeEditor::BeginNode( nodeAId );
    {
        ImGui::Text( "Node A" );
        NodeEditor::BeginPin( nodeAInputPinId, NodeEditor::PinKind::Input );
        {
            ImGui::Text( "-> In" );
        }
        NodeEditor::EndPin();
        ImGui::SameLine();
        NodeEditor::BeginPin( nodeAOutputPinId, NodeEditor::PinKind::Output );
        {
            ImGui::Text( "Out ->" );
        }
        NodeEditor::EndPin();
    }
    NodeEditor::EndNode();

    // Node B
    NodeEditor::NodeId nodeBId = uniqueId++;
    NodeEditor::PinId nodeBInputPinId1 = uniqueId++;
    NodeEditor::PinId nodeBInputPinId2 = uniqueId++;
    NodeEditor::PinId nodeBOutputPinId = uniqueId++;
    if ( m_firstFrame ) {
        NodeEditor::SetNodePosition( nodeBId, ImVec2( 210, 60 ) );
    }
    NodeEditor::BeginNode( nodeBId );
    {
        ImGui::Text( "Node B" );
        ImGui::ex::BeginColumn();
        {
            NodeEditor::BeginPin( nodeBInputPinId1, NodeEditor::PinKind::Input );
            {
                ImGui::Text( "-> In1" );
            }
            NodeEditor::EndPin();
            NodeEditor::BeginPin( nodeBInputPinId2, NodeEditor::PinKind::Input );
            {
                ImGui::Text( "-> In2" );
            }
            NodeEditor::EndPin();
        }
        ImGui::ex::NextColumn();
        {
            ImGui::SameLine();
            NodeEditor::BeginPin( nodeBOutputPinId, NodeEditor::PinKind::Output );
            {
                ImGui::Text( "Out ->" );
            }
            NodeEditor::EndPin();
        }
        ImGui::ex::EndColumn();
    }
    NodeEditor::EndNode();

    for ( auto &linkInfo : m_links ) {
        NodeEditor::Link( linkInfo.id, linkInfo.inputPinId, linkInfo.outputPinId );
    }

    // Handle interactions
    // Handle creation action, returns true if editor want to create a new object (node or link)
    if ( NodeEditor::BeginCreate() ) {
        NodeEditor::PinId inputPinId;
        NodeEditor::PinId outputPinId;
        if ( NodeEditor::QueryNewLink( &inputPinId, &outputPinId ) ) {
            if ( inputPinId && outputPinId ) {
                if ( NodeEditor::AcceptNewItem() ) {
                    const LinkInfo newLink = {
                        .id = NodeEditor::LinkId( m_nextLinkId++ ),
                        .inputPinId = inputPinId,
                        .outputPinId = outputPinId,
                    };
                    m_links.push_back( newLink );
                    NodeEditor::Link( newLink.id, newLink.inputPinId, newLink.outputPinId );
                }
            }
        }
    }
    NodeEditor::EndCreate();

    // End of interaction with editor
    NodeEditor::End();

    if ( m_firstFrame ) {
        NodeEditor::NavigateToContent( 0.0f );
    }

    NodeEditor::SetCurrentEditor( nullptr );

    m_firstFrame = false;
}
