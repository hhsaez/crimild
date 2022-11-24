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

#include "Editor/Menus/viewMenu.hpp"

#include "Editor/EditorLayer.hpp"
#include "Editor/Layout.hpp"
#include "Editor/Panels/BehaviorEditorPanel.hpp"
#include "Editor/Panels/NodeInspectorPanel.hpp"
#include "Editor/Panels/RenderScenePanel.hpp"
#include "Editor/Panels/SceneHierarchyPanel.hpp"
#include "Editor/Panels/ScenePanel.hpp"
#include "Editor/Panels/SimulationPanel.hpp"
#include "Foundation/ImGUIUtils.hpp"

using namespace crimild;

static void addPanel( EditorLayer *editor, std::shared_ptr< editor::layout::Panel > const &newPanel ) noexcept
{
    auto newLayout = crimild::alloc< editor::layout::Layout >( editor::layout::Layout::Direction::LEFT, editor::layout::Layout::Fraction( 0.25 ) );
    newLayout->setFirst( newPanel );
    newLayout->setSecond( editor->getLayout() );
    editor->setLayout( newLayout );
}

void crimild::editor::viewMenu( EditorLayer *editor ) noexcept
{
    if ( ImGui::BeginMenu( "View" ) ) {
        if ( ImGui::MenuItem( "Scene Hierarchy..." ) ) {
            addPanel( editor, crimild::alloc< SceneHierarchyPanel >() );
        }

        if ( ImGui::MenuItem( "Node Inspector..." ) ) {
            addPanel( editor, crimild::alloc< NodeInspectorPanel >( editor->getRenderDevice() ) );
        }

        ImGui::Separator();

        if ( ImGui::MenuItem( "Scene..." ) ) {
            addPanel( editor, crimild::alloc< ScenePanel >( editor->getRenderDevice() ) );
        }

        if ( ImGui::MenuItem( "Simulation..." ) ) {
            addPanel( editor, crimild::alloc< SimulationPanel >( editor->getRenderDevice() ) );
        }

        ImGui::Separator();

        if ( ImGui::MenuItem( "Behavior Editor..." ) ) {
            addPanel( editor, crimild::alloc< BehaviorEditorPanel >() );
        }

        ImGui::Separator();

        if ( ImGui::MenuItem( "Render..." ) ) {
            addPanel( editor, crimild::alloc< RenderScenePanel >( editor->getRenderDevice() ) );
        }

        ImGui::EndMenu();
    }
}

