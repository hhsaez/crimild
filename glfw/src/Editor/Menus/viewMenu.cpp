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

#include "Behaviors/Actions/PrintMessage.hpp"
#include "Behaviors/Actions/Rotate.hpp"
#include "Behaviors/BehaviorController.hpp"
#include "Behaviors/BehaviorTree.hpp"
#include "Behaviors/Composites/Composite.hpp"
#include "Behaviors/Composites/Sequence.hpp"
#include "Behaviors/Decorators/Decorator.hpp"
#include "Behaviors/Decorators/Repeat.hpp"
#include "Components/MaterialComponent.hpp"
#include "Editor/EditorLayer.hpp"
#include "Foundation/ImGUIUtils.hpp"
#include "Foundation/Version.hpp"
#include "Mathematics/Matrix4_inverse.hpp"
#include "Mathematics/Transformation_apply.hpp"
#include "Mathematics/Transformation_translation.hpp"
#include "Mathematics/get_ptr.hpp"
#include "Rendering/Materials/PrincipledBSDFMaterial.hpp"
#include "Rendering/Materials/UnlitMaterial.hpp"
#include "SceneGraph/CSGNode.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Group.hpp"
#include "Simulation/Settings.hpp"
#include "Simulation/Simulation.hpp"
#include "Visitors/NodeVisitor.hpp"
#include "Visitors/UpdateWorldState.hpp"

using namespace crimild;

void behaviorEditor( bool &open, EditorLayer *editor )
{
    // Graph datas
    static const GraphEditor::Template mTemplates[] = {
        {
            IM_COL32( 160, 160, 180, 255 ),
            IM_COL32( 100, 100, 140, 255 ),
            IM_COL32( 110, 110, 150, 255 ),
            1,
            ( Array { "MyInput" } ).getData(),
            nullptr,
            2,
            ( Array { "MyOutput0", "MyOuput1" } ).getData(),
            nullptr,
        },

        {
            IM_COL32( 180, 160, 160, 255 ),
            IM_COL32( 140, 100, 100, 255 ),
            IM_COL32( 150, 110, 110, 255 ),
            3,
            nullptr,
            ( Array { IM_COL32( 200, 100, 100, 255 ), IM_COL32( 100, 200, 100, 255 ), IM_COL32( 100, 100, 200, 255 ) } ).getData(),
            1,
            ( Array { "MyOutput0" } ).getData(),
            ( Array { IM_COL32( 200, 200, 200, 255 ) } ).getData(),
        },
    };

    struct GraphEditorDelegate : public GraphEditor::Delegate {

        void configure( behaviors::BehaviorController *behaviors )
        {
            if ( crimild::get_ptr( m_behaviors ) == behaviors ) {
                return;
            }

            m_behaviors = behaviors;

            m_nodes.clear();
            m_links.clear();
            m_index.clear();

            if ( m_behaviors == nullptr ) {
                return;
            }

            configure( behaviors->getBehaviorTree() );
        }

        void configure( behaviors::BehaviorTree *tree )
        {
            m_index[ tree->getUniqueID() ] = m_nodes.size();
            m_nodes.push_back(
                Node {
                    .type = Node::Type::EVENT,
                    .name = tree->getName(),
                    .tree = crimild::retain( tree ),
                } );

            auto root = tree->getRootBehavior();
            if ( root == nullptr ) {
                return;
            }

            size_t rootIdx;
            if ( configure( root, rootIdx ) ) {
                m_links.push_back( { 0, 0, rootIdx, 0 } );
            }
        }

        bool configure( behaviors::Behavior *behavior, size_t &idx )
        {
            if ( behavior == nullptr ) {
                return false;
            }

            m_index[ behavior->getUniqueID() ] = m_nodes.size();
            m_nodes.push_back(
                Node {
                    .type = Node::Type::ACTION,
                    .name = getBehaviorName( behavior->getClassName() ),
                    .behavior = crimild::retain( behavior ),
                } );
            idx = m_index[ behavior->getUniqueID() ];

            return true;
        }

        std::string getBehaviorName( std::string className ) const
        {
            auto pos = className.find_last_of( ":" );
            if ( pos != std::string::npos ) {
                return className.substr( pos + 1 );
            } else {
                return className;
            }
        }

        void addBehavior( SharedPointer< behaviors::Behavior > const &behavior )
        {
            size_t idx;
            configure( crimild::get_ptr( behavior ), idx );
        }

        bool AllowedLink( GraphEditor::NodeIndex from, GraphEditor::NodeIndex to ) override
        {
            return true;
        }

        void SelectNode( GraphEditor::NodeIndex nodeIndex, bool selected ) override
        {
            m_nodes[ nodeIndex ].mSelected = selected;
        }

        void MoveSelectedNodes( const ImVec2 delta ) override
        {
            for ( auto &node : m_nodes ) {
                if ( !node.mSelected ) {
                    continue;
                }
                node.x += delta.x;
                node.y += delta.y;
            }
        }

        virtual void RightClick( GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput ) override
        {
        }

        void AddLink( GraphEditor::NodeIndex inputNodeIndex, GraphEditor::SlotIndex inputSlotIndex, GraphEditor::NodeIndex outputNodeIndex, GraphEditor::SlotIndex outputSlotIndex ) override
        {
            std::cout << inputNodeIndex << " " << inputSlotIndex << " " << outputNodeIndex << " " << outputSlotIndex << std::endl;

            auto &in = m_nodes[ inputNodeIndex ];
            auto &out = m_nodes[ outputNodeIndex ];

            if ( in.type == Node::Type::EVENT ) {
                // Remove existing links
                auto it = std::find_if(
                    m_links.begin(),
                    m_links.end(),
                    [ & ]( auto l ) {
                        return l.mInputNodeIndex == inputNodeIndex;
                    } );
                if ( it != m_links.end() ) {
                    m_links.erase( it );
                }

                in.tree->setRootBehavior( out.behavior );
            }

            m_links.push_back( { inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex } );
        }

        void DelLink( GraphEditor::LinkIndex linkIndex ) override
        {
            m_links.erase( m_links.begin() + linkIndex );
        }

        void CustomDraw( ImDrawList *drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex ) override
        {
            drawList->AddLine( rectangle.Min, rectangle.Max, IM_COL32( 0, 0, 0, 255 ) );
            drawList->AddText( rectangle.Min, IM_COL32( 255, 128, 64, 255 ), "Draw" );
        }

        const size_t GetTemplateCount() override
        {
            return sizeof( mTemplates ) / sizeof( GraphEditor::Template );
        }

        const GraphEditor::Template GetTemplate( GraphEditor::TemplateIndex index ) override
        {
            return mTemplates[ index ];
        }

        const size_t GetNodeCount() override
        {
            return m_nodes.size();
        }

        const GraphEditor::Node GetNode( GraphEditor::NodeIndex index ) override
        {
            const auto &node = m_nodes[ index ];

            return GraphEditor::Node {
                node.name.c_str(),
                node.templateIndex,
                ImRect( ImVec2( node.x, node.y ), ImVec2( node.x + 200, node.y + 200 ) ),
                node.mSelected
            };
        }

        const size_t GetLinkCount() override
        {
            return m_links.size();
        }

        const GraphEditor::Link GetLink( GraphEditor::LinkIndex index ) override
        {
            return m_links[ index ];
        }

        behaviors::BehaviorController *m_behaviors = nullptr;

        struct Node {
            enum Type {
                EVENT,
                DECORATOR,
                ACTION,
                COMPOSITE,
            };

            std::string name;
            Type type;

            // TODO: It would be ideal to use a union here, but it makes things more complex to declare.
            SharedPointer< behaviors::BehaviorTree > tree;
            SharedPointer< behaviors::Behavior > behavior;

            // const char *name;
            GraphEditor::TemplateIndex templateIndex;
            float x = 0;
            float y = 0;
            bool mSelected = false;
        };

        std::unordered_map< size_t, size_t > m_index;
        std::vector< Node > m_nodes;
        std::vector< GraphEditor::Link > m_links;

        // std::vector< Node > mNodes = {
        //     { "My Node 0",
        //       0,
        //       0,
        //       0,
        //       false },

        //     { "My Node 1",
        //       0,
        //       400,
        //       0,
        //       false },

        //     { "My Node 2",
        //       1,
        //       400,
        //       400,
        //       false }
        // };

        // std::vector< GraphEditor::Link > mLinks = { { 0, 0, 1, 0 } };
    };

    static GraphEditor::Options options;
    static GraphEditorDelegate delegate;
    static GraphEditor::ViewState viewState;
    static GraphEditor::FitOnScreen fit = GraphEditor::Fit_None;

    if ( !open ) {
        return;
    }

    auto selected = editor->getSelectedNode();
    if ( selected == nullptr ) {
        return;
    }

    delegate.configure( editor->getSelectedNode()->getComponent< behaviors::BehaviorController >() );

    if ( ImGui::Begin( "Behaviors", &open, 0 ) ) {
        if ( ImGui::Button( "Fit All Nodes" ) ) {
            fit = GraphEditor::Fit_AllNodes;
        }

        ImGui::SameLine();

        if ( ImGui::Button( "Fit Selected Nodes" ) ) {
            fit = GraphEditor::Fit_SelectedNodes;
        }

        if ( ImGui::Button( "Add Seq..." ) ) {
            delegate.addBehavior( crimild::alloc< behaviors::composites::Sequence >() );
        }

        ImGui::SameLine();

        if ( ImGui::Button( "Add Print..." ) ) {
            delegate.addBehavior( crimild::alloc< behaviors::actions::PrintMessage >() );
        }

        ImGui::SameLine();

        if ( ImGui::Button( "Add Rot X..." ) ) {
            delegate.addBehavior( crimild::alloc< behaviors::actions::Rotate >( Vector3 { 1, 0, 0 }, 0.5 ) );
        }

        ImGui::SameLine();

        if ( ImGui::Button( "Add Rot Y..." ) ) {
            delegate.addBehavior( crimild::alloc< behaviors::actions::Rotate >( Vector3 { 0, 1, 0 }, 0.5 ) );
        }

        ImGui::SameLine();

        if ( ImGui::Button( "Add Rep 3..." ) ) {
            delegate.addBehavior( crimild::alloc< behaviors::decorators::Repeat >( 3 ) );
        }

        GraphEditor::Show( delegate, options, viewState, true, &fit );

        ImGui::End();
    }
}

void materialComponentDetails( MaterialComponent *materials )
{
    auto material = materials->first();
    if ( material == nullptr ) {
        return;
    }

    ImGui::Text( "Material (%s)", material->getClassName() );

    if ( material->getClassName() == materials::PrincipledBSDF::__CLASS_NAME ) {
        const auto bsdf = static_cast< materials::PrincipledBSDF * >( material );
        auto albedo = bsdf->getAlbedo();
        ImGui::ColorEdit3( "Albedo", get_ptr( albedo ) );
        bsdf->setAlbedo( albedo );
    } else {
        const auto unlit = static_cast< UnlitMaterial * >( material );
        auto color = rgb( unlit->getColor() );
        ImGui::ColorEdit3( "Color", get_ptr( color ) );
        // return std::make_tuple( unlit->getColor(), unlit->getTexture() );
    }
}

void nodeComponentsSection( Node *node )
{
    ImGui::SetNextItemOpen( true );
    if ( ImGui::CollapsingHeader( "Components", nullptr ) ) {
        node->forEachComponent(
            []( NodeComponent *cmp ) {
                if ( cmp->getClassName() == MaterialComponent::__CLASS_NAME ) {
                    materialComponentDetails( static_cast< MaterialComponent * >( cmp ) );
                }
            } );
    }
}

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

            Point3 nodeTranslation;
            Vector3 nodeRotation;
            Vector3 nodeScale;
            ImGuizmo::DecomposeMatrixToComponents( get_ptr( node->getLocal().mat ), get_ptr( nodeTranslation ), get_ptr( nodeRotation ), get_ptr( nodeScale ) );

            bool changed = false;
            changed = changed || ImGui::InputFloat3( "Tr", get_ptr( nodeTranslation ) );
            changed = changed || ImGui::InputFloat3( "Rt", get_ptr( nodeRotation ) );
            changed = changed || ImGui::InputFloat3( "Sc", get_ptr( nodeScale ) );
            if ( changed ) {
                Matrix4 mat;
                ImGuizmo::RecomposeMatrixFromComponents( get_ptr( nodeTranslation ), get_ptr( nodeRotation ), get_ptr( nodeScale ), get_ptr( mat ) );
                node->setLocal( Transformation { mat, inverse( mat ) } );
                node->perform( UpdateWorldState() );
            }

            nodeComponentsSection( node );
        } else {
            ImGui::Text( "No node selected" );
        }
    }
    ImGui::End();
}

class SceneTreeBuilder : public NodeVisitor {
public:
    explicit SceneTreeBuilder( EditorLayer *editor )
        : m_editor( editor )
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
            true );
    }

    void visitGroup( Group *group ) override
    {
        visit(
            group,
            [ & ] { NodeVisitor::visitGroup( group ); },
            group->getNodeCount() == 0 );
    }

    void visitCSGNode( CSGNode *csg ) override
    {
        visit(
            csg,
            [ & ] { NodeVisitor::visitCSGNode( csg ); },
            csg->getLeft() == nullptr && csg->getRight() == nullptr );
    }

private:
    std::string getNodeName( Node *node ) noexcept
    {
        return !node->getName().empty()
                   ? node->getName()
                   : node->getClassName();
    }

    template< typename NodeType, typename Fn >
    void visit( NodeType *node, Fn fn, bool isLeaf )
    {
        auto flags = m_baseFlags;
        if ( m_editor->getSelectedNode() == node ) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        if ( isLeaf ) {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        ImGui::PushID( node->getUniqueID() );
        if ( ImGui::TreeNodeEx( getNodeName( node ).c_str(), flags ) ) {
            if ( ImGui::IsItemClicked() ) {
                m_editor->setSelectedNode( node );
            }
            fn();
            if ( !isLeaf ) {
                ImGui::TreePop();
            }
        }
        ImGui::PopID();
    }

private:
    EditorLayer *m_editor = nullptr;
    Size m_ptrId = 0;
    ImGuiTreeNodeFlags m_baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
};

void sceneHierarchyPanel( bool &open, EditorLayer *editor )
{
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
    static bool showBehaviorEditor = false;

    if ( ImGui::BeginMenu( "View" ) ) {
        if ( ImGui::MenuItem( "Scene Hierarchy..." ) ) {
            showSceneHierarchyPanel = true;
        }

        if ( ImGui::MenuItem( "Behavior Editor..." ) ) {
            showBehaviorEditor = true;
        }
        ImGui::EndMenu();
    }

    sceneHierarchyPanel( showSceneHierarchyPanel, editor );
    nodeInspectorPanel( showNodeInspectorPanel, editor );
    behaviorEditor( showBehaviorEditor, editor );
}
