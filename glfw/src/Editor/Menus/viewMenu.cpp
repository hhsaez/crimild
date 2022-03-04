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
#include "Foundation/Version.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "SceneGraph/CSGNode.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/NodeVisitor.hpp"
#include "Visitors/UpdateWorldState.hpp"
#include "imgui.h"

using namespace crimild;

void nodeInspectorPanel( bool &open, EditorLayer *editor ) noexcept
{
    if ( !open ) {
        return;
    }

    ImGui::SetNextWindowPos( ImVec2( 200, 200 ), ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowSize( ImVec2( 200, 300 ), ImGuiCond_FirstUseEver );

    if ( ImGui::Begin( "Node Inspector", &open, ImGuiWindowFlags_NoCollapse ) ) {
        auto node = editor->getSelectedNode();
        if ( node != nullptr ) {
            ImGui::Text( "Class: %s", node->getClassName() );
            ImGui::Text( "ID: %llu", node->getUniqueID() );

            Point3 pos = location( node->getLocal() );
            if ( ImGui::InputFloat3( "Position", &pos.x, 6, 0 ) ) {
                node->setLocal( translation( pos.x, pos.y, pos.z ) );
                node->perform( UpdateWorldState() );
            }
        } else {
            ImGui::Text( "No node selected" );
        }
    }
    ImGui::End();
}

void sceneHierarchyPanel( bool &open, EditorLayer *editor )
{
    class SceneTreeBuilder : public NodeVisitor {
    public:
        explicit SceneTreeBuilder( EditorLayer *editor )
            : m_editor( editor )
        {
        }

        ~SceneTreeBuilder( void ) = default;

        void visitNode( Node *node ) override
        {
            if ( ImGui::TreeNodeEx( getNodeName( node ).c_str() ) ) {
                if ( ImGui::IsItemClicked() ) {
                    m_editor->setSelectedNode( node );
                }
                ImGui::TreePop();
            }
        }

        void visitGroup( Group *group ) override
        {
            if ( ImGui::TreeNode( getNodeName( group ).c_str() ) ) {
                if ( ImGui::IsItemClicked() ) {
                    m_editor->setSelectedNode( group );
                }
                NodeVisitor::visitGroup( group );
                ImGui::TreePop();
            }
        }

        void visitCSGNode( CSGNode *csg ) override
        {
            if ( ImGui::TreeNode( getNodeName( csg ).c_str() ) ) {
                if ( ImGui::IsItemClicked() ) {
                    m_editor->setSelectedNode( csg );
                }
                NodeVisitor::visitCSGNode( csg );
                ImGui::TreePop();
            }
        }

    private:
        std::string getNodeName( Node *node ) noexcept
        {
            return !node->getName().empty()
                       ? node->getName()
                       : node->getClassName();
        }

    private:
        EditorLayer *m_editor = nullptr;
        Size m_ptrId = 0;
        ImGuiTreeNodeFlags m_baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    };

    if ( !open ) {
        return;
    }

    ImGui::SetNextWindowPos( ImVec2( 20, 40 ), ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowSize( ImVec2( 280, 500 ), ImGuiCond_FirstUseEver );

    if ( ImGui::Begin( "Scene", &open, ImGuiWindowFlags_NoCollapse ) ) {
        if ( Simulation::getInstance() != nullptr ) {
            auto scene = Simulation::getInstance()->getScene();
            if ( scene ) {
                scene->perform( SceneTreeBuilder( editor ) );
            } else {
                ImGui::Text( "No valid scene" );
            }
        } else {
            ImGui::Text( "No Simulation instance found" );
        }
        ImGui::Text( "" ); // padding
    }

    ImGui::End();
}

void crimild::editor::viewMenu( EditorLayer *editor ) noexcept
{
    static bool showSceneHierarchyPanel = true;
    static bool showNodeInspectorPanel = true;

    if ( ImGui::BeginMenu( "View" ) ) {
        if ( ImGui::MenuItem( "Scene Hierarchy..." ) ) {
            showSceneHierarchyPanel = true;
        }
        ImGui::EndMenu();
    }

    sceneHierarchyPanel( showSceneHierarchyPanel, editor );
    nodeInspectorPanel( showNodeInspectorPanel, editor );
}
