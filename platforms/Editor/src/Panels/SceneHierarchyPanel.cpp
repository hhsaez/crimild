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

#include "Panels/SceneHierarchyPanel.hpp"

#include "Foundation/ImGuiUtils.hpp"
#include "SceneGraph/PrefabNode.hpp"
#include "Simulation/Editor.hpp"

#include <Crimild.hpp>

using namespace crimild;
using namespace crimild::editor::panels;

class SceneTreeBuilder : public NodeVisitor {
public:
    SceneTreeBuilder( void ) noexcept
    {
        // Makes sure the root node is always expanded.
        ImGui::SetNextItemOpen( true, ImGuiCond_Once );
    }

    ~SceneTreeBuilder( void ) = default;

    void visitNode( Node *node ) override
    {
        visit(
            node,
            [] {},
            true,
            false
        );
    }

    void visitGroup( Group *group ) override
    {
        auto color = ColorRGB { 1, 1, 1 };
        auto modified = false;

        if ( auto prefab = dynamic_cast< PrefabNode * >( group ) ) {
            color = ColorRGB { 0, 0.5, 1 };
            if ( prefab->isLinked() && !prefab->isEditable() ) {
                visit(
                    group,
                    [] {},
                    true,
                    false,
                    color
                );
                return;
            } else if ( !prefab->isLinked() ) {
                color = ColorRGB { 0.75, 0.85, 1 };
            } else if ( prefab->isEditable() ) {
                modified = true;
            }
        }

        visit(
            group,
            [ & ] {
                group->forEachNode(
                    [ & ]( Node *node ) { node->accept( *this ); },
                    // Include disabled nodes too
                    false
                );
            },
            group->getNodeCount() == 0,
            true,
            color,
            modified
        );
    }

    void visitCSGNode( CSGNode *csg ) override
    {
        visit(
            csg,
            [ & ] { NodeVisitor::visitCSGNode( csg ); },
            csg->getLeft() == nullptr && csg->getRight() == nullptr,
            false
        );
    }

private:
    std::string getNodeName( Node *node ) noexcept
    {
        return !node->getName().empty()
                   ? node->getName()
                   : node->getClassName();
    }

    template< typename NodeType, typename Fn >
    void visit(
        NodeType *node,
        Fn fn,
        bool isLeaf,
        bool isDropTarget,
        const ColorRGB &color = ColorRGB { 1, 1, 1 },
        bool modified = false
    )
    {
        auto editor = editor::Editor::getInstance();
        auto selected = editor->getSelectedObject< Node >();

        auto flags = m_baseFlags;
        if ( selected == node ) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        if ( isLeaf ) {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        } else {
            flags |= ImGuiTreeNodeFlags_OpenOnArrow;
        }

        ImGui::PushID( node->getUniqueID() );
        ImGui::PushStyleColor(
            ImGuiCol_Text,
            node->isEnabled()
                ? IM_COL32( color.r * 255, color.g * 255, color.b * 255, 255 )
                : IM_COL32( color.r * 255, color.g * 255, color.b * 255, 64 )
        );

        auto treeNodeName = getNodeName( node ) + ( modified ? " (*)" : "" );

        const auto isOpen = ImGui::TreeNodeEx( treeNodeName.c_str(), flags );

        ImGui::PopStyleColor();
        ImGui::PopID();

        if ( ImGui::BeginDragDropSource( ImGuiDragDropFlags_None ) ) {
            // Set payload as node address since sending pointers directly is buggy
            auto nodeAddr = size_t( node );
            ImGui::SetDragDropPayload( "DND_NODE", &nodeAddr, sizeof( nodeAddr ) );
            // DragDrop tooltip
            ImGui::Text( "%s", getNodeName( node ).c_str() );
            ImGui::EndDragDropSource();
        }

        if ( ImGui::BeginPopupContextItem() ) {
            if ( ImGui::Selectable( "Clone" ) ) {
                editor::Editor::getInstance()->cloneNode( node );
            }
            ImGui::Separator();
            if ( ImGui::Selectable( "Delete" ) ) {
                editor::Editor::getInstance()->deleteNode( node );
            }
            ImGui::EndPopup();
        }

        if ( isDropTarget ) {
            if ( ImGui::BeginDragDropTarget() ) {
                if ( auto payload = ImGui::AcceptDragDropPayload( "DND_NODE" ) ) {
                    auto nodeAddr = *( ( size_t * ) payload->Data );
                    Node *other = reinterpret_cast< Node * >( nodeAddr );
                    if ( other != nullptr ) {
                        auto child = other->detachFromParent();
                        if ( auto group = dynamic_cast< Group * >( node ) ) {
                            group->attachNode( child );
                        } else {
                            CRIMILD_LOG_WARNING( "Drop target node is not a group" );
                        }
                    }
                } else if ( auto payload = ImGui::AcceptDragDropPayload( "DND_FILE_PATH" ) ) {
                    std::string path( static_cast< const char * >( payload->Data ), payload->DataSize );
                    auto prefab = crimild::alloc< PrefabNode >( path );
                    if ( auto group = dynamic_cast< Group * >( node ) ) {
                        group->attachNode( prefab );
                        prefab->perform( StartComponents() );
                        prefab->perform( UpdateWorldState() );
                    } else {
                        CRIMILD_LOG_WARNING( "Drop target node is not a group" );
                    }
                }
                ImGui::EndDragDropTarget();
            }
        }

        // Handles selection by using mouse hover+released instead of click
        // because click is triggered on mouse down and will conflict with
        // drag/drop events.
        if ( ImGui::IsItemHovered() ) {
            if ( ImGui::IsMouseReleased( 0 ) ) {
                editor->setSelectedObject( node );
            }
        }

        if ( isOpen ) {
            fn();
            if ( !isLeaf ) {
                ImGui::TreePop();
            }
        }
    }

private:
    ImGuiTreeNodeFlags m_baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
};

void SceneHierarchy::render( void ) noexcept
{
    bool open = true;

    ImGui::Begin( "Scene Hierarchy", &open, 0 );
    if ( Simulation::getInstance() != nullptr ) {
        auto scene = Simulation::getInstance()->getScene();
        if ( scene ) {
            scene->perform( SceneTreeBuilder() );
        } else {
            ImGui::Text( "No valid scene" );
        }
    } else {
        ImGui::Text( "No Simulation instance found" );
    }

    ImGui::End();
}
