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

#include "Editor/Panels/BehaviorEditorPanel.hpp"

#include "Behaviors/Actions/PrintMessage.hpp"
#include "Behaviors/Actions/Rotate.hpp"
#include "Behaviors/BehaviorController.hpp"
#include "Behaviors/BehaviorTree.hpp"
#include "Behaviors/Composites/Composite.hpp"
#include "Behaviors/Composites/Sequence.hpp"
#include "Behaviors/Decorators/Decorator.hpp"
#include "Behaviors/Decorators/Repeat.hpp"
#include "Behaviors/withBehavior.hpp"
#include "Editor/EditorLayer.hpp"
#include "Foundation/ImGUIUtils.hpp"
#include "Mathematics/Vector2Ops.hpp"

using namespace crimild;

static void behaviorEditor( bool &open, int panelId, behaviors::BehaviorController *controller )
{
    struct GraphEditorDelegate : public GraphEditor::Delegate {
        struct Node {
            enum Type {
                TREE,
                DECORATOR,
                ACTION,
                COMPOSITE,
            };

            std::string name;
            Type type;

            union {
                behaviors::BehaviorTree *tree = nullptr;
                behaviors::composites::Composite *composite;
                behaviors::decorators::Decorator *decorator;
                behaviors::Behavior *action;
            };

            // const char *name;
            GraphEditor::TemplateIndex templateIndex = 0;
            float x = 0;
            float y = 0;
            bool mSelected = false;
        };

        void configure( behaviors::BehaviorController *behaviors )
        {
            if ( crimild::get_ptr( m_behaviors ) == behaviors ) {
                return;
            }

            m_behaviors = behaviors;

            m_nodes.clear();
            m_links.clear();
            m_index.clear();
            m_templates.clear();
            m_allBehaviors.clear();

            if ( m_behaviors == nullptr ) {
                return;
            }

            configure( behaviors->getBehaviorTree(), Vector2 { 0, 300 } );
        }

        void configure( behaviors::BehaviorTree *tree, const Vector2 &offset )
        {
            m_index[ tree->getUniqueID() ] = m_nodes.size();
            m_nodes.push_back(
                Node {
                    .type = Node::Type::TREE,
                    .name = tree->getName(),
                    .tree = tree,
                    .x = offset.x,
                    .y = offset.y,
                    .templateIndex = m_templates.size(),
                } );
            m_templates.emplace_back( getBehaviorTreeTemplate() );

            auto root = tree->getRootBehavior();
            if ( root == nullptr ) {
                return;
            }

            size_t rootIdx;
            if ( configure( root, offset + Vector2 { 300, 0 }, rootIdx ) ) {
                m_links.push_back( { 0, 0, rootIdx, 0 } );
            }
        }

        bool configure( behaviors::Behavior *behavior, const Vector2 offset, size_t &idx )
        {
            if ( behavior == nullptr ) {
                return false;
            }

            m_allBehaviors.push_back( crimild::retain( behavior ) );

            idx = m_nodes.size();
            m_index[ behavior->getUniqueID() ] = idx;
            m_nodes.push_back(
                Node {
                    .name = getBehaviorName( behavior->getClassName() ),
                    .templateIndex = m_templates.size(),
                    .x = offset.x,
                    .y = offset.y,
                } );

            if ( auto composite = dynamic_cast< behaviors::composites::Composite * >( behavior ) ) {
                m_nodes[ idx ].type = Node::Type::COMPOSITE;
                m_nodes[ idx ].composite = composite;
                m_templates.push_back( getBehaviorCompositeTemplate() );
                const auto N = composite->getBehaviorCount();
                for ( auto i = 0l; i < N; ++i ) {
                    size_t childIdx;
                    if ( configure( composite->getBehaviorAt( i ), offset + Vector2 { 300.0f, 300.0f * i }, childIdx ) ) {
                        m_links.push_back( { idx, 0, childIdx, 0 } );
                    }
                }
            } else if ( auto decorator = dynamic_cast< behaviors::decorators::Decorator * >( behavior ) ) {
                m_nodes[ idx ].type = Node::Type::DECORATOR;
                m_nodes[ idx ].decorator = decorator;
                m_templates.push_back( getBehaviorDecoratorTemplate() );
                if ( auto childBehavior = decorator->getBehavior() ) {
                    size_t childIdx;
                    if ( configure( childBehavior, offset + Vector2 { 300, 0 }, childIdx ) ) {
                        m_links.push_back( { idx, 0, childIdx, 0 } );
                    }
                }
            } else {
                m_nodes[ idx ].type = Node::Type::ACTION;
                m_nodes[ idx ].action = behavior;
                m_templates.push_back( getBehaviorActionTemplate() );
            }

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
            configure( crimild::get_ptr( behavior ), Vector2 { 0, 0 }, idx );
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
            auto &in = m_nodes[ inputNodeIndex ];
            auto &out = m_nodes[ outputNodeIndex ];

            auto getOutBehavior = [ & ]() -> SharedPointer< behaviors::Behavior > {
                if ( out.composite ) {
                    return crimild::retain( out.composite );
                } else if ( out.decorator ) {
                    return crimild::retain( out.decorator );
                } else {
                    return crimild::retain( out.action );
                }
            };

            auto delCurrentLink = [ & ] {
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
            };

            if ( in.type == Node::Type::TREE ) {
                delCurrentLink();
                in.tree->setRootBehavior( getOutBehavior() );
            } else if ( in.type == Node::Type::COMPOSITE ) {
                in.composite->attachBehavior( getOutBehavior() );
            } else if ( in.type == Node::Type::DECORATOR ) {
                delCurrentLink();
                in.decorator->setBehavior( getOutBehavior() );
            }

            m_links.push_back( { inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex } );
        }

        void DelLink( GraphEditor::LinkIndex linkIndex ) override
        {
            auto link = m_links[ linkIndex ];
            auto in = m_nodes[ link.mInputNodeIndex ];
            auto out = m_nodes[ link.mOutputNodeIndex ];

            auto getOutBehavior = [ & ]() -> SharedPointer< behaviors::Behavior > {
                if ( out.composite ) {
                    return crimild::retain( out.composite );
                } else if ( out.decorator ) {
                    return crimild::retain( out.decorator );
                } else {
                    return crimild::retain( out.action );
                }
            };

            if ( in.type == Node::Type::TREE ) {
                in.tree->setRootBehavior( nullptr );
            } else if ( in.type == Node::Type::COMPOSITE ) {
                in.composite->detachBehavior( getOutBehavior() );
            } else if ( in.type == Node::Type::DECORATOR ) {
                in.decorator->setBehavior( nullptr );
            }
            m_links.erase( m_links.begin() + linkIndex );
        }

        void CustomDraw( ImDrawList *drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex ) override
        {
            drawList->AddLine( rectangle.Min, rectangle.Max, IM_COL32( 0, 0, 0, 255 ) );
            drawList->AddText( rectangle.Min, IM_COL32( 255, 128, 64, 255 ), "Draw" );
        }

        const size_t GetTemplateCount() override
        {
            return m_templates.size();
        }

        const GraphEditor::Template GetTemplate( GraphEditor::TemplateIndex index ) override
        {
            return m_templates[ index ];
        }

        GraphEditor::Template getBehaviorTreeTemplate( void ) const
        {
            return {
                IM_COL32( 160, 160, 180, 255 ),
                IM_COL32( 140, 140, 100, 255 ),
                IM_COL32( 150, 150, 110, 255 ),
                0,
                nullptr,
                nullptr,
                1,
                nullptr,
                nullptr,
            };
        }

        GraphEditor::Template getBehaviorActionTemplate( void ) const
        {
            return {
                IM_COL32( 160, 160, 180, 255 ),
                IM_COL32( 140, 100, 100, 255 ),
                IM_COL32( 150, 110, 110, 255 ),
                1,
                nullptr,
                nullptr,
                0,
                nullptr,
                nullptr,
            };
        }

        GraphEditor::Template getBehaviorDecoratorTemplate( void ) const
        {
            return {
                IM_COL32( 160, 160, 180, 255 ),
                IM_COL32( 100, 100, 140, 255 ),
                IM_COL32( 110, 110, 150, 255 ),
                1,
                nullptr,
                nullptr,
                1,
                nullptr,
                nullptr,
            };
        }

        GraphEditor::Template getBehaviorCompositeTemplate( void ) const
        {
            return {
                IM_COL32( 160, 160, 180, 255 ),
                IM_COL32( 100, 140, 100, 255 ),
                IM_COL32( 110, 150, 110, 255 ),
                1,
                nullptr,
                nullptr,
                1,
                nullptr,
                nullptr,
            };
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

    private:
        behaviors::BehaviorController *m_behaviors = nullptr;
        std::unordered_map< size_t, size_t > m_index;
        std::vector< Node > m_nodes;
        std::vector< GraphEditor::Link > m_links;
        std::vector< GraphEditor::Template > m_templates;
        std::vector< SharedPointer< behaviors::Behavior > > m_allBehaviors;
    };

    static GraphEditor::Options options;
    static GraphEditorDelegate delegate;
    static GraphEditor::ViewState viewState;
    static GraphEditor::FitOnScreen fit = GraphEditor::Fit_None;

    if ( !open ) {
        return;
    }

    delegate.configure( controller );

    // Allow opening multiple panels with the same name
    std::stringstream ss;
    ss << "Behaviors##" << panelId;

    ImGui::SetNextWindowPos( ImVec2( 310, 750 ), ImGuiCond_FirstUseEver );
    ImGui::SetNextWindowSize( ImVec2( 1280, 325 ), ImGuiCond_FirstUseEver );

    if ( ImGui::Begin( ss.str().c_str(), &open, 0 ) ) {
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

        if ( ImGui::Button( "Add Print A..." ) ) {
            delegate.addBehavior( crimild::alloc< behaviors::actions::PrintMessage >( "A" ) );
        }

        ImGui::SameLine();

        if ( ImGui::Button( "Add Print B..." ) ) {
            delegate.addBehavior( crimild::alloc< behaviors::actions::PrintMessage >( "B" ) );
        }

        ImGui::SameLine();

        if ( ImGui::Button( "Add Print C..." ) ) {
            delegate.addBehavior( crimild::alloc< behaviors::actions::PrintMessage >( "C" ) );
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

void BehaviorEditorPanel::render( void ) noexcept
{
    bool open = true;

    auto editor = EditorLayer::getInstance();

    auto controller = [ & ]() -> behaviors::BehaviorController * {
        auto selected = editor->getSelectedNode();
        if ( selected == nullptr ) {
            return nullptr;
        }
        return selected->getComponent< behaviors::BehaviorController >();
    }();

    behaviorEditor( open, ( size_t ) this, controller );

    if ( !open ) {
        detachFromParent();
        return;
    }

    Layer::render();
}
