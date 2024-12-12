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

namespace nd = ax::NodeEditor;

GraphEditorWindow::GraphEditorWindow( void ) noexcept
    : Window( "Graph Editor" )
{
    ax::NodeEditor::Config config;
    config.SettingsFile = "./NodeEditor.json";
    m_context = ax::NodeEditor::CreateEditor( &config );
}

GraphEditorWindow::~GraphEditorWindow( void ) noexcept
{
    ax::NodeEditor::DestroyEditor( m_context );
}

void GraphEditorWindow::drawContent( void ) noexcept
{
    auto &io = ImGui::GetIO();

    ImGui::Text( "FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0 );

    ImGui::Separator();

    nd::SetCurrentEditor( m_context );

    nd::Begin( "My Editor", ImVec2( 0.0f, 0.0f ) );
    int uniqueId = 1;

    nd::BeginNode( uniqueId++ );
    ImGui::Text( "-> In" );
    nd::BeginPin( uniqueId++, nd::PinKind::Input );
    nd::EndPin();
    ImGui::SameLine();
    nd::BeginPin( uniqueId++, nd::PinKind::Output );
    ImGui::Text( "Out ->" );
    nd::EndPin();
    nd::EndNode();

    nd::End();

    nd::SetCurrentEditor( nullptr );
}
