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

#include "Views/Windows/GraphEditor/GraphEditorWindow.hpp"

#include "Views/Windows/GraphEditor/Builders.hpp"

using namespace crimild;
using namespace crimild::editor;

namespace NodeEditor = ax::NodeEditor;

static inline ImRect ImGui_GetItemRect( void ) noexcept
{
    return ImRect( ImGui::GetItemRectMin(), ImGui::GetItemRectMax() );
}

static inline ImRect ImRect_Expanded( const ImRect &rect, float x, float y ) noexcept
{
    auto result = rect;
    result.Min.x -= x;
    result.Min.y -= y;
    result.Max.x += x;
    result.Max.y += y;
    return result;
}

static int getTextureWidth( ImTextureID texture ) noexcept
{
    if ( texture == nullptr ) {
        return 0;
    }
    return 1; // TODO
}

static int getTextureHeight( ImTextureID texture ) noexcept
{
    if ( texture == nullptr ) {
        return 0;
    }
    return 1; // TODO
}

#if !GRAPH_EDITOR_BLUEPRINTS

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

#endif

bool splitter( bool splitVertically, float thickness, float *size1, float *size2, float minSize1, float minSize2, float splitterLongAxisSize = -1.0f ) noexcept
{
    using namespace ImGui;
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    ImGuiID id = window->GetID( "##Splitter" );
    ImRect bb;
    bb.Min = window->DC.CursorPos + ( splitVertically ? ImVec2( *size1, 0 ) : ImVec2( 0, *size1 ) );
    bb.Max = bb.Min + CalcItemSize( splitVertically ? ImVec2( thickness, splitterLongAxisSize ) : ImVec2( splitterLongAxisSize, thickness ), 0, 0 );
    return SplitterBehavior( bb, id, splitVertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, minSize1, minSize2, 0 );
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

#if GRAPH_EDITOR_BLUEPRINTS

    updateTouch();

    // static NodeEditor::NodeId contextNodeId = 0;
    // static NodeEditor::LinkId contextLinkId = 0;
    // static NodeEditor::PinId contextPinId = 0;

    // TODO(hernan): Move these to member variables?
    // static bool createNewNode = false;
    // static Pin *newNodeLinkPin = nullptr;
    // static Pin *newLinkPin = nullptr;

    // TODO(hernan): Move these to member variables?
    static float leftPanelWidth = 400.0f;
    static float rightPanelWidth = 800.0f;

    splitter( true, 4.0f, &leftPanelWidth, &rightPanelWidth, 50.0f, 50.0f );

    showLeftPanel( leftPanelWidth - 4.0f );

    ImGui::SameLine( 0.0f, 12.0f );

    NodeEditor::Begin( "Node Editor" );

    auto cursorTopLeft = ImGui::GetCursorScreenPos();
    crimild::editor::utils::AssemblyNodeBuilder builder( m_headerBackground, getTextureWidth( m_headerBackground ), getTextureHeight( m_headerBackground ) );

    renderBlueprintAndSimpleNodes( builder );
    renderTreeNodes();
    renderHoudiniNodes();
    renderCommentNodes();
    renderLinks();
    renderCreateNewNode();

    ImGui::SetCursorScreenPos( cursorTopLeft );

    // Popups
    renderPopups();

    // End of interaction with editor
    NodeEditor::End();

    showOrdinals();

    NodeEditor::SetCurrentEditor( nullptr );
#else
    int uniqueId = 1;

    NodeEditor::Begin( "Node Editor" );

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
    // NodeEditor::NodeId nodeAId = uniqueId++;
    // NodeEditor::PinId nodeAInputPinId = uniqueId++;
    // NodeEditor::PinId nodeAOutputPinId = uniqueId++;
    // if ( m_firstFrame ) {
    //     NodeEditor::SetNodePosition( nodeAId, ImVec2( 10, 10 ) );
    // }
    // NodeEditor::BeginNode( nodeAId );
    // {
    //     ImGui::Text( "Node A" );
    //     NodeEditor::BeginPin( nodeAInputPinId, NodeEditor::PinKind::Input );
    //     {
    //         ImGui::Text( "-> In" );
    //     }
    //     NodeEditor::EndPin();
    //     ImGui::SameLine();
    //     NodeEditor::BeginPin( nodeAOutputPinId, NodeEditor::PinKind::Output );
    //     {
    //         ImGui::Text( "Out ->" );
    //     }
    //     NodeEditor::EndPin();
    // }
    // NodeEditor::EndNode();

    // // Node B
    // NodeEditor::NodeId nodeBId = uniqueId++;
    // NodeEditor::PinId nodeBInputPinId1 = uniqueId++;
    // NodeEditor::PinId nodeBInputPinId2 = uniqueId++;
    // NodeEditor::PinId nodeBOutputPinId = uniqueId++;
    // if ( m_firstFrame ) {
    //     NodeEditor::SetNodePosition( nodeBId, ImVec2( 210, 60 ) );
    // }
    // NodeEditor::BeginNode( nodeBId );
    // {
    //     ImGui::Text( "Node B" );
    //     ImGui::ex::BeginColumn();
    //     {
    //         NodeEditor::BeginPin( nodeBInputPinId1, NodeEditor::PinKind::Input );
    //         {
    //             ImGui::Text( "-> In1" );
    //         }
    //         NodeEditor::EndPin();
    //         NodeEditor::BeginPin( nodeBInputPinId2, NodeEditor::PinKind::Input );
    //         {
    //             ImGui::Text( "-> In2" );
    //         }
    //         NodeEditor::EndPin();
    //     }
    //     ImGui::ex::NextColumn();
    //     {
    //         ImGui::SameLine();
    //         NodeEditor::BeginPin( nodeBOutputPinId, NodeEditor::PinKind::Output );
    //         {
    //             ImGui::Text( "Out ->" );
    //         }
    //         NodeEditor::EndPin();
    //     }
    //     ImGui::ex::EndColumn();
    // }
    // NodeEditor::EndNode();

    // for ( auto &linkInfo : m_links ) {
    //     NodeEditor::Link( linkInfo.id, linkInfo.inputPinId, linkInfo.outputPinId );
    // }

    // // Handle interactions
    // // Handle creation action, returns true if editor want to create a new object (node or link)
    // if ( NodeEditor::BeginCreate() ) {
    //     NodeEditor::PinId inputPinId;
    //     NodeEditor::PinId outputPinId;
    //     if ( NodeEditor::QueryNewLink( &inputPinId, &outputPinId ) ) {
    //         if ( inputPinId && outputPinId ) {
    //             if ( NodeEditor::AcceptNewItem() ) {
    //                 const LinkInfo newLink = {
    //                     .id = NodeEditor::LinkId( m_nextLinkId++ ),
    //                     .inputPinId = inputPinId,
    //                     .outputPinId = outputPinId,
    //                 };
    //                 m_links.push_back( newLink );
    //                 NodeEditor::Link( newLink.id, newLink.inputPinId, newLink.outputPinId );
    //             }
    //         }
    //     }
    // }
    // NodeEditor::EndCreate();

    NodeEditor::End();

    if ( m_firstFrame ) {
        NodeEditor::NavigateToContent( 0.0f );
    }

    m_firstFrame = false;

    NodeEditor::SetCurrentEditor( nullptr );
#endif
}

#if GRAPH_EDITOR_BLUEPRINTS
void GraphEditorWindow::updateTouch( void ) noexcept
{
    const auto deltaTime = ImGui::GetIO().DeltaTime;
    for ( auto &entry : m_nodeTouchTime ) {
        if ( entry.second > 0.0f ) {
            entry.second -= deltaTime;
        }
    }
}

void GraphEditorWindow::showLeftPanel( float panelWidth ) noexcept
{
    // TODO
}

void GraphEditorWindow::drawPinIcon( const Pin &pin, bool connected, int alpha ) const noexcept
{
    assert( false && "Missing implementation" );
}

GraphNode *GraphEditorWindow::findNode( NodeEditor::NodeId id ) noexcept
{
    for ( auto &node : m_nodes ) {
        if ( node.id == id ) {
            return &node;
        }
    }
    return nullptr;
}

Link *GraphEditorWindow::findLink( NodeEditor::LinkId id ) noexcept
{
    for ( auto &link : m_links ) {
        if ( link.id == id ) {
            return &link;
        }
    }
    return nullptr;
}

Pin *GraphEditorWindow::findPin( NodeEditor::PinId id ) noexcept
{
    if ( !id ) {
        return nullptr;
    }

    for ( auto &node : m_nodes ) {
        for ( auto &pin : node.inputs ) {
            if ( pin.id == id ) {
                return &pin;
            }
        }
        for ( auto &pin : node.outputs ) {
            if ( pin.id == id ) {
                return &pin;
            }
        }
    }

    return nullptr;
}

GraphNode *GraphEditorWindow::spawnInputActionNode( void ) noexcept
{
    return nullptr;
}
GraphNode *GraphEditorWindow::spawnOutputActionNode( void ) noexcept
{
    return nullptr;
}

GraphNode *GraphEditorWindow::spawnBranchNode( void ) noexcept
{
    return nullptr;
}

GraphNode *GraphEditorWindow::spawnDoNNode( void ) noexcept
{
    return nullptr;
}

GraphNode *GraphEditorWindow::spawnSetTimerNode( void ) noexcept
{
    return nullptr;
}

GraphNode *GraphEditorWindow::spawnLessNode( void ) noexcept
{
    return nullptr;
}

GraphNode *GraphEditorWindow::spawnWeirdNode( void ) noexcept
{
    return nullptr;
}

GraphNode *GraphEditorWindow::spawnTraceByChannelNode( void ) noexcept
{
    return nullptr;
}

GraphNode *GraphEditorWindow::spawnPrintStringNode( void ) noexcept
{
    return nullptr;
}

GraphNode *GraphEditorWindow::spawnCommentNode( void ) noexcept
{
    return nullptr;
}

GraphNode *GraphEditorWindow::spawnTreeSequenceNode( void ) noexcept
{
    return nullptr;
}

GraphNode *GraphEditorWindow::spawnTreeMoveToNode( void ) noexcept
{
    return nullptr;
}

GraphNode *GraphEditorWindow::spawnTreeRandomWaitNode( void ) noexcept
{
    return nullptr;
}

GraphNode *GraphEditorWindow::spawnMessageNode( void ) noexcept
{
    return nullptr;
}

GraphNode *GraphEditorWindow::spawnHoudiniTransformNode( void ) noexcept
{
    return nullptr;
}

GraphNode *GraphEditorWindow::spawnHoudiniGroupNode( void ) noexcept
{
    return nullptr;
}

ImColor GraphEditorWindow::getIconColor( PinType type ) const noexcept
{
    switch ( type ) {
        default:
        case PinType::Flow:
            return ImColor( 255, 255, 255 );
        case PinType::Bool:
            return ImColor( 220, 48, 48 );
        case PinType::Int:
            return ImColor( 68, 201, 156 );
        case PinType::Float:
            return ImColor( 147, 226, 74 );
        case PinType::String:
            return ImColor( 124, 21, 153 );
        case PinType::Object:
            return ImColor( 51, 150, 215 );
        case PinType::Function:
            return ImColor( 218, 0, 183 );
        case PinType::Delegate:
            return ImColor( 255, 48, 48 );
    }
}

void GraphEditorWindow::renderBlueprintAndSimpleNodes( utils::AssemblyNodeBuilder &builder ) noexcept
{
    // Renders NodeType::Blueprint || NodeType::Simple
    for ( auto &node : m_nodes ) {
        if ( node.type != NodeType::Blueprint && node.type != NodeType::Simple ) {
            continue;
        }

        const auto isSimple = node.type == NodeType::Simple;

        bool hasOutputDelegates = false;
        for ( auto &output : node.outputs ) {
            if ( output.type == PinType::Delegate ) {
                hasOutputDelegates = true;
            }
        }

        builder.begin( node.id );
        {
            if ( !isSimple ) {
                builder.header( node.color );
                {
                    ImGui::Spring( 0 );
                    ImGui::TextUnformatted( node.name.c_str() );
                    ImGui::Spring( 1 );
                    ImGui::Dummy( ImVec2( 0, 28 ) );
                    if ( hasOutputDelegates ) {
                        ImGui::BeginVertical( "delegates", ImVec2( 0, 28 ) );
                        ImGui::Spring( 1, 0 );
                        for ( auto &output : node.outputs ) {
                            if ( output.type != PinType::Delegate ) {
                                continue;
                            }

                            auto alpha = ImGui::GetStyle().Alpha;
                            if ( m_newLinkPin && !canCreateLink( m_newLinkPin, &output ) && &output != m_newLinkPin ) {
                                alpha = alpha * ( 48.0f / 255.0f );
                            }

                            NodeEditor::BeginPin( output.id, NodeEditor::PinKind::Output );
                            NodeEditor::PinPivotAlignment( ImVec2( 1.0f, 0.5f ) );
                            NodeEditor::PinPivotSize( ImVec2( 0, 0 ) );
                            ImGui::BeginHorizontal( output.id.AsPointer() );
                            ImGui::PushStyleVar( ImGuiStyleVar_Alpha, alpha );
                            if ( !output.name.empty() ) {
                                ImGui::TextUnformatted( output.name.c_str() );
                                ImGui::Spring( 0 );
                            }
                            drawPinIcon( output, isPinkLinked( output.id ), ( int ) ( alpha * 255 ) );
                            ImGui::Spring( 0, ImGui::GetStyle().ItemSpacing.x / 2 );
                            ImGui::EndHorizontal();
                            ImGui::PopStyleVar();
                        }
                        ImGui::Spring( 1, 0 );
                        ImGui::EndVertical();
                        ImGui::Spring( 0, ImGui::GetStyle().ItemInnerSpacing.x / 2 );
                    } else {
                        ImGui::Spring( 0 );
                    }
                }
                builder.endHeader();
            }

            for ( auto &input : node.inputs ) {
                auto alpha = ImGui::GetStyle().Alpha;
                if ( m_newLinkPin && !canCreateLink( m_newLinkPin, &input ) && &input != m_newLinkPin ) {
                    alpha *= 48.0f / 255.0f;
                }

                builder.input( input.id );
                ImGui::PushStyleVar( ImGuiStyleVar_Alpha, alpha );
                drawPinIcon( input, isPinkLinked( input.id ), ( int ) ( alpha * 255 ) );
                ImGui::Spring( 0 );
                if ( !input.name.empty() ) {
                    ImGui::TextUnformatted( input.name.c_str() );
                    ImGui::Spring( 0 );
                }
                if ( input.type == PinType::Bool ) {
                    ImGui::Button( "Hello" );
                    ImGui::Spring( 0 );
                }
                ImGui::PopStyleVar();
                builder.endInput();
            }

            if ( isSimple ) {
                builder.middle();
                ImGui::Spring( 1, 0 );
                ImGui::TextUnformatted( node.name.c_str() );
                ImGui::Spring( 1, 0 );
            }

            for ( auto &output : node.outputs ) {
                if ( !isSimple && output.type == PinType::Delegate ) {
                    continue;
                }

                auto alpha = ImGui::GetStyle().Alpha;
                if ( m_newLinkPin && !canCreateLink( m_newLinkPin, &output ) && &output != m_newLinkPin ) {
                    alpha *= 48.0f / 255.0f;
                }

                ImGui::PushStyleVar( ImGuiStyleVar_Alpha, alpha );
                builder.output( output.id );
                if ( output.type == PinType::String ) {
                    static char buffer[ 128 ] = "Edit Me\nMultiline!";
                    static bool wasActive = false;

                    ImGui::PushItemWidth( 100.0f );
                    ImGui::InputText( "##edit", buffer, 127 );
                    ImGui::PopItemWidth();

                    if ( ImGui::IsItemActive() && !wasActive ) {
                        NodeEditor::EnableShortcuts( false );
                        wasActive = true;
                    } else if ( !ImGui::IsItemActive() && wasActive ) {
                        NodeEditor::EnableShortcuts( true );
                        wasActive = false;
                    }
                    ImGui::Spring( 0 );
                }
                if ( !output.name.empty() ) {
                    ImGui::Spring( 0 );
                    ImGui::TextUnformatted( output.name.c_str() );
                }
                ImGui::Spring( 0 );
                drawPinIcon( output, isPinkLinked( output.id ), ( int ) ( alpha * 255 ) );
                ImGui::PopStyleVar();
                builder.endOutput();
            }
        }
        builder.end();
    }
}

void GraphEditorWindow::renderTreeNodes( void ) noexcept
{
    // Renders NodeType::Tree
    for ( auto &node : m_nodes ) {
        if ( node.type != NodeType::Tree ) {
            continue;
        }

        const float rounding = 5.0f;
        const float padding = 12.0f;

        const auto pinBackground = NodeEditor::GetStyle().Colors[ NodeEditor::StyleColor_NodeBg ];

        NodeEditor::PushStyleColor( NodeEditor::StyleColor_NodeBg, ImColor( 128, 128, 128, 200 ) );
        NodeEditor::PushStyleColor( NodeEditor::StyleColor_NodeBorder, ImColor( 32, 32, 32, 200 ) );
        NodeEditor::PushStyleColor( NodeEditor::StyleColor_PinRect, ImColor( 60, 180, 255, 150 ) );
        NodeEditor::PushStyleColor( NodeEditor::StyleColor_PinRectBorder, ImColor( 60, 180, 255, 150 ) );

        NodeEditor::PushStyleVar( NodeEditor::StyleVar_NodePadding, ImVec4( 0, 0, 0, 0 ) );
        NodeEditor::PushStyleVar( NodeEditor::StyleVar_NodeRounding, rounding );
        NodeEditor::PushStyleVar( NodeEditor::StyleVar_SourceDirection, ImVec2( 0, 1 ) );
        NodeEditor::PushStyleVar( NodeEditor::StyleVar_TargetDirection, ImVec2( 0, -1 ) );
        NodeEditor::PushStyleVar( NodeEditor::StyleVar_LinkStrength, 0.0f );
        NodeEditor::PushStyleVar( NodeEditor::StyleVar_PinBorderWidth, 1.0f );
        NodeEditor::PushStyleVar( NodeEditor::StyleVar_PinRadius, 5.0f );

        ImGui::BeginVertical( node.id.AsPointer() );
        ImGui::BeginHorizontal( "inputs" );
        ImGui::Spring( 0, padding * 2 );
        ImRect inputsRect;
        int inputAlpha = 200;
        if ( !node.inputs.empty() ) {
            auto &pin = node.inputs[ 0 ];
            ImGui::Dummy( ImVec2( 0, padding ) );
            ImGui::Spring( 1, 0 );
            inputsRect = ImGui_GetItemRect();

            NodeEditor::PushStyleVar( NodeEditor::StyleVar_PinArrowSize, 10.0f );
            NodeEditor::PushStyleVar( NodeEditor::StyleVar_PinArrowWidth, 10.0f );
    #if IMGUI_VERSION_NUM > 18101
            NodeEditor::PushStyleVar( NodeEditor::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom );
    #else
            NodeEditor::PushStyleVar( NodeEditor::StyleVar_PinCorners, 12 );
    #endif
            NodeEditor::BeginPin( pin.id, NodeEditor::PinKind::Input );
            NodeEditor::PinPivotRect( inputsRect.GetTL(), inputsRect.GetBR() );
            NodeEditor::PinRect( inputsRect.GetTL(), inputsRect.GetBR() );
            NodeEditor::EndPin();
            NodeEditor::PopStyleVar( 3 );

            if ( m_newLinkPin && !canCreateLink( m_newLinkPin, &pin ) && &pin != m_newLinkPin ) {
                inputAlpha = ( int ) ( 255 * ImGui::GetStyle().Alpha * ( 48.0f / 255.0f ) );
            }
        } else {
            ImGui::Dummy( ImVec2( 0, padding ) );
        }
        ImGui::Spring( 0, padding * 2 );
        ImGui::EndHorizontal();

        ImGui::BeginHorizontal( "content_frame" );
        ImGui::Spring( 1, padding );
        ImGui::BeginVertical( "content", ImVec2( 0, 0 ) );
        ImGui::Dummy( ImVec2( 160, 0 ) );
        ImGui::Spring( 1 );
        ImGui::TextUnformatted( node.name.c_str() );
        ImGui::Spring( 1 );
        ImGui::EndVertical();
        auto contentRect = ImGui_GetItemRect();
        ImGui::Spring( 1, padding );
        ImGui::EndHorizontal();

        ImGui::BeginHorizontal( "otuputs" );
        ImGui::Spring( 0, padding * 2 );
        ImRect outputsRect;
        int outputAlpha = 255;
        if ( !node.outputs.empty() ) {
            auto &pin = node.outputs[ 0 ];
            ImGui::Dummy( ImVec2( 0, padding ) );
            ImGui::Spring( 1, 0 );
            outputsRect = ImGui_GetItemRect();
    #if IMGUI_VERSION_NUM > 18101
            NodeEditor::PushStyleVar( NodeEditor::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom );
    #else
            NodeEditor::PushStyleVar( NodeEditor::StyleVar_PinCorners, 3 );
    #endif
            NodeEditor::BeginPin( pin.id, NodeEditor::PinKind::Output );
            NodeEditor::PinPivotRect( outputsRect.GetTL(), outputsRect.GetBR() );
            NodeEditor::PinRect( outputsRect.GetTL(), outputsRect.GetBR() );
            NodeEditor::EndPin();
            NodeEditor::PopStyleVar();

            if ( m_newLinkPin && !canCreateLink( m_newLinkPin, &pin ) && &pin != m_newLinkPin ) {
                outputAlpha = ( int ) ( 255 * ImGui::GetStyle().Alpha * ( 48.0f / 255.0f ) );
            }
        } else {
            ImGui::Dummy( ImVec2( 0, padding ) );
        }
        ImGui::Spring( 0, padding * 2 );
        ImGui::EndHorizontal();

        ImGui::EndVertical();

        NodeEditor::EndNode();
        NodeEditor::PopStyleVar( 7 );
        NodeEditor::PopStyleColor( 4 );

        auto drawList = NodeEditor::GetNodeBackgroundDrawList( node.id );

    #if IMGUI_VERSION_NUM > 18101
        const auto topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
        const auto bottomRoundCornersFlags = ImDrawFlags_RoundCornersBottom;
    #else
        const auto topRoundCornersFlags = 1 | 2;
        const auto bottomRoundCornersFlags = 4 | 8;
    #endif
        drawList->AddRectFilled(
            inputsRect.GetTL() + ImVec2( 0, 1 ),
            inputsRect.GetBR(),
            IM_COL32( ( int ) ( 255 * pinBackground.x ), ( int ) ( 255 * pinBackground.y ), ( int ) ( 255 * pinBackground.z ), inputAlpha ),
            4.0f,
            bottomRoundCornersFlags
        );
        drawList->AddRect(
            inputsRect.GetTL() + ImVec2( 0, 1 ),
            inputsRect.GetBR(),
            IM_COL32( ( int ) ( 255 * pinBackground.x ), ( int ) ( 255 * pinBackground.y ), ( int ) ( 255 * pinBackground.z ), inputAlpha ),
            4.0f,
            bottomRoundCornersFlags
        );
        drawList->AddRectFilled(
            outputsRect.GetTL(),
            outputsRect.GetBR() - ImVec2( 0, 1 ),
            IM_COL32( ( int ) ( 255 * pinBackground.x ), ( int ) ( 255 * pinBackground.y ), ( int ) ( 255 * pinBackground.z ), inputAlpha ),
            4.0f,
            topRoundCornersFlags
        );
        drawList->AddRect(
            outputsRect.GetTL(),
            outputsRect.GetBR() - ImVec2( 0, 1 ),
            IM_COL32( ( int ) ( 255 * pinBackground.x ), ( int ) ( 255 * pinBackground.y ), ( int ) ( 255 * pinBackground.z ), inputAlpha ),
            4.0f,
            topRoundCornersFlags
        );
        drawList->AddRectFilled(
            contentRect.GetTL(),
            contentRect.GetBR(),
            IM_COL32( 24, 64, 128, 200 ),
            0.0f
        );
        drawList->AddRect(
            contentRect.GetTL(),
            contentRect.GetBR(),
            IM_COL32( 48, 128, 255, 100 ),
            0.0f
        );
    }
}

void GraphEditorWindow::renderHoudiniNodes( void ) noexcept
{
    // Renders NodeType::Houdini
    for ( auto &node : m_nodes ) {
        if ( node.type != NodeType::Houdini ) {
            continue;
        }

        const float rounding = 10.0f;
        const float padding = 12.0f;

        NodeEditor::PushStyleColor( NodeEditor::StyleColor_NodeBg, ImColor( 229, 229, 229, 200 ) );
        NodeEditor::PushStyleColor( NodeEditor::StyleColor_NodeBorder, ImColor( 125, 125, 125, 200 ) );
        NodeEditor::PushStyleColor( NodeEditor::StyleColor_PinRect, ImColor( 229, 229, 229, 60 ) );
        NodeEditor::PushStyleColor( NodeEditor::StyleColor_PinRectBorder, ImColor( 125, 125, 125, 60 ) );

        const auto pinBackground = NodeEditor::GetStyle().Colors[ NodeEditor::StyleColor_NodeBg ];

        NodeEditor::PushStyleVar( NodeEditor::StyleVar_NodePadding, ImVec4( 0, 0, 0, 0 ) );
        NodeEditor::PushStyleVar( NodeEditor::StyleVar_NodeRounding, rounding );
        NodeEditor::PushStyleVar( NodeEditor::StyleVar_SourceDirection, ImVec2( 0, 1 ) );
        NodeEditor::PushStyleVar( NodeEditor::StyleVar_TargetDirection, ImVec2( 0, -1 ) );
        NodeEditor::PushStyleVar( NodeEditor::StyleVar_LinkStrength, 0.0f );
        NodeEditor::PushStyleVar( NodeEditor::StyleVar_PinBorderWidth, 1.0f );
        NodeEditor::PushStyleVar( NodeEditor::StyleVar_PinRadius, 6.0f );

        NodeEditor::BeginNode( node.id );
        ImGui::BeginVertical( node.id.AsPointer() );
        if ( !node.inputs.empty() ) {
            ImGui::BeginHorizontal( "inputs" );
            ImGui::Spring( 1, 0 );
            ImRect inputsRect;
            int inputAlpha = 200;
            for ( auto &pin : node.inputs ) {
                ImGui::Dummy( ImVec2( padding, padding ) );
                inputsRect = ImGui_GetItemRect();
                ImGui::Spring( 1, 0 );
                inputsRect.Min.y -= padding;
                inputsRect.Max.y -= padding;

    #if IMGUI_VERSION_NUM > 18101
                const auto allRoundCornersFlags = ImDrawFlags_RoundCornersAll;
    #else
                const auto allRoundCornersFlags = 15;
    #endif
                NodeEditor::PushStyleVar( NodeEditor::StyleVar_PinCorners, allRoundCornersFlags );
                NodeEditor::BeginPin( pin.id, NodeEditor::PinKind::Input );
                NodeEditor::PinPivotRect( inputsRect.GetCenter(), inputsRect.GetCenter() );
                NodeEditor::PinRect( inputsRect.GetTL(), inputsRect.GetBR() );
                NodeEditor::EndPin();
                NodeEditor::PopStyleVar( 1 );

                auto drawList = ImGui::GetWindowDrawList();
                drawList->AddRectFilled(
                    inputsRect.GetTL(),
                    inputsRect.GetBR(),
                    IM_COL32( ( int ) ( 255 * pinBackground.x ), ( int ) ( 255 * pinBackground.y ), ( int ) ( 255 * pinBackground.z ), inputAlpha ),
                    4.0f,
                    allRoundCornersFlags
                );
                drawList->AddRect(
                    inputsRect.GetTL(),
                    inputsRect.GetBR(),
                    IM_COL32( ( int ) ( 255 * pinBackground.x ), ( int ) ( 255 * pinBackground.y ), ( int ) ( 255 * pinBackground.z ), inputAlpha ),
                    4.0f,
                    allRoundCornersFlags
                );

                if ( m_newLinkPin && !canCreateLink( m_newLinkPin, &pin ) && &pin != m_newLinkPin ) {
                    inputAlpha = ( int ) ( 255 * ImGui::GetStyle().Alpha * ( 48.0f / 255.0f ) );
                }
            }
            ImGui::EndHorizontal();
        }
        ImGui::BeginHorizontal( "content_frame" );
        ImGui::Spring( 1, padding );
        ImGui::BeginVertical( "content", ImVec2( 0.0f, 0.0f ) );
        ImGui::Dummy( ImVec2( 160, 0 ) );
        ImGui::Spring( 1 );
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0, 0, 0, 1 ) );
        ImGui::TextUnformatted( node.name.c_str() );
        ImGui::PopStyleColor();
        ImGui::Spring( 1 );
        ImGui::EndVertical();

        auto contentRect = ImGui_GetItemRect();

        ImGui::Spring( 1, padding );
        ImGui::EndHorizontal();

        if ( !node.outputs.empty() ) {
            ImGui::BeginHorizontal( "outputs" );
            ImGui::Spring( 1, 0 );

            ImRect outputsRect;
            int inputAlpha = 200;
            for ( auto &pin : node.outputs ) {
                ImGui::Dummy( ImVec2( padding, padding ) );
                outputsRect = ImGui_GetItemRect();
                ImGui::Spring( 1, 0 );
                outputsRect.Min.y += padding;
                outputsRect.Max.y += padding;

    #if IMGUI_VERSION_NUM > 18101
                const auto allRoundCornersFlags = ImDrawFlags_RoundCornersAll;
                const auto topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
    #else
                const auto allRoundCornersFlags = 15;
                const auto topRoundCornersFlags = 3;
    #endif

                NodeEditor::PushStyleVar( NodeEditor::StyleVar_PinCorners, topRoundCornersFlags );
                NodeEditor::BeginPin( pin.id, NodeEditor::PinKind::Output );
                NodeEditor::PinPivotRect( outputsRect.GetCenter(), outputsRect.GetCenter() );
                NodeEditor::PinRect( outputsRect.GetTL(), outputsRect.GetBR() );
                NodeEditor::EndPin();
                NodeEditor::PopStyleVar();

                auto drawList = ImGui::GetWindowDrawList();
                drawList->AddRectFilled(
                    outputsRect.GetTL(),
                    outputsRect.GetBR(),
                    IM_COL32( ( int ) ( 255 * pinBackground.x ), ( int ) ( 255 * pinBackground.y ), ( int ) ( 255 * pinBackground.z ), inputAlpha ),
                    4.0f,
                    allRoundCornersFlags
                );
                drawList->AddRect(
                    outputsRect.GetTL(),
                    outputsRect.GetBR(),
                    IM_COL32( ( int ) ( 255 * pinBackground.x ), ( int ) ( 255 * pinBackground.y ), ( int ) ( 255 * pinBackground.z ), inputAlpha ),
                    4.0f,
                    allRoundCornersFlags
                );

                if ( m_newLinkPin && canCreateLink( m_newLinkPin, &pin ) && &pin != m_newLinkPin ) {
                    inputAlpha = ( int ) ( 255 * ImGui::GetStyle().Alpha * ( 48.0f / 255.0f ) );
                }
            }
            ImGui::EndHorizontal();
        }
        ImGui::EndVertical();

        NodeEditor::EndNode();
        NodeEditor::PopStyleVar( 7 );
        NodeEditor::PopStyleColor( 4 );
    }
}

void GraphEditorWindow::renderCommentNodes( void ) noexcept
{
    for ( auto &node : m_nodes ) {
        if ( node.type != NodeType::Comment ) {
            continue;
        }

        const float commentAlpha = 0.75f;

        ImGui::PushStyleVar( ImGuiStyleVar_Alpha, commentAlpha );
        NodeEditor::PushStyleColor( NodeEditor::StyleColor_NodeBg, ImColor( 255, 255, 255, 64 ) );
        NodeEditor::PushStyleColor( NodeEditor::StyleColor_NodeBorder, ImColor( 255, 255, 255, 64 ) );
        NodeEditor::BeginNode( node.id );
        ImGui::PushID( node.id.AsPointer() );
        ImGui::BeginVertical( "content" );
        ImGui::BeginHorizontal( "horizontal" );
        ImGui::Spring( 1 );
        ImGui::TextUnformatted( node.name.c_str() );
        ImGui::Spring( 1 );
        ImGui::EndHorizontal();
        NodeEditor::Group( node.size );
        ImGui::EndVertical();
        ImGui::PopID();
        NodeEditor::EndNode();
        NodeEditor::PopStyleColor( 2 );
        ImGui::PopStyleVar();

        if ( NodeEditor::BeginGroupHint( node.id ) ) {
            auto bgAlpha = static_cast< int >( ImGui::GetStyle().Alpha * 255 );

            auto min = NodeEditor::GetGroupMin();

            ImGui::SetCursorScreenPos( min - ImVec2( -8, ImGui::GetTextLineHeightWithSpacing() + 4 ) );
            ImGui::BeginGroup();
            ImGui::TextUnformatted( node.name.c_str() );
            ImGui::EndGroup();

            auto drawList = NodeEditor::GetHintBackgroundDrawList();
            auto hintBounds = ImGui_GetItemRect();
            auto hintFrameBounds = ImRect_Expanded( hintBounds, 8, 4 );

            drawList->AddRectFilled(
                hintFrameBounds.GetTL(),
                hintFrameBounds.GetBR(),
                IM_COL32( 255, 255, 255, 64 * bgAlpha / 255 ),
                4.0f
            );
            drawList->AddRect(
                hintFrameBounds.GetTL(),
                hintFrameBounds.GetBR(),
                IM_COL32( 255, 255, 255, 128 * bgAlpha / 255 ),
                4.0f
            );
        }
        NodeEditor::EndGroupHint();
    }
}

void GraphEditorWindow::renderLinks( void ) noexcept
{
    for ( auto &link : m_links ) {
        NodeEditor::Link( link.id, link.startPinId, link.endPinId, link.color, 2.0f );
    }
}

void GraphEditorWindow::renderCreateNewNode( void ) noexcept
{
    if ( !m_createNewNode ) {
        if ( NodeEditor::BeginCreate( ImColor( 255, 255, 255 ), 2.0f ) ) {
            // TODO: Move this outside
            auto showLabel = []( const char *label, ImColor color ) {
                ImGui::SetCursorPosY( ImGui::GetCursorPosY() - ImGui::GetTextLineHeight() );
                auto size = ImGui::CalcTextSize( label );
                auto padding = ImGui::GetStyle().FramePadding;
                auto spacing = ImGui::GetStyle().ItemSpacing;
                ImGui::SetCursorPos( ImGui::GetCursorPos() + ImVec2( spacing.x, -spacing.y ) );
                auto rectMin = ImGui::GetCursorScreenPos() - padding;
                auto rectMax = ImGui::GetCursorScreenPos() + size - padding;
                auto drawList = ImGui::GetWindowDrawList();
                drawList->AddRectFilled( rectMin, rectMax, color, size.y * 0.15f );
                ImGui::TextUnformatted( label );
            };

            NodeEditor::PinId startPinId = 0;
            NodeEditor::PinId endPinId = 0;

            if ( NodeEditor::QueryNewLink( &startPinId, &endPinId ) ) {
                auto startPin = findPin( startPinId );
                auto endPin = findPin( endPinId );
                m_newLinkPin = startPin ? startPin : endPin;
                if ( startPin->kind == PinKind::Input ) {
                    // WARNING: StartPin could be null?
                    std::swap( startPin, endPin );
                    std::swap( startPinId, endPinId );
                }

                if ( startPin && endPin ) {
                    if ( endPin == startPin ) {
                        NodeEditor::RejectNewItem( ImColor( 255, 0, 0 ), 2.0f );

                    } else if ( endPin->kind == startPin->kind ) {
                        showLabel( "x Incomtabile Pin Kind", ImColor( 45, 32, 32, 180 ) );
                        NodeEditor::RejectNewItem( ImColor( 255, 0, 0 ), 2.0f );
                    } else if ( endPin->node == startPin->node ) [[unlikely]] {
                        showLabel( "x Cannot connecto to self", ImColor( 45, 32, 32, 180 ) );
                        NodeEditor::RejectNewItem( ImColor( 255, 0, 0 ), 2.0f );
                    } else if ( endPin->type != startPin->type ) {
                        showLabel( "x Incompatible Pin Type", ImColor( 45, 32, 32, 180 ) );
                        NodeEditor::RejectNewItem( ImColor( 255.0f, 128.0f, 128.0f, 1.0f ) );
                    } else {
                        showLabel( "+ Create Link", ImColor( 32, 45, 32, 180 ) );
                        if ( NodeEditor::AcceptNewItem( ImColor( 128, 255, 128 ), 4.0f ) ) {
                            m_links.emplace_back( Link( getNextId(), startPinId, endPinId ) );
                            m_links.back().color = getIconColor( startPin->type );
                        }
                    }
                }
            }

            NodeEditor::PinId pinId = 0;
            if ( NodeEditor::QueryNewNode( &pinId ) ) {
                m_newLinkPin = findPin( pinId );
                if ( m_newLinkPin ) {
                    showLabel( "+ Create New", ImColor( 32, 45, 32, 180 ) );
                }

                if ( NodeEditor::AcceptNewItem() ) {
                    m_createNewNode = true;
                    m_newNodeLinkPin = findPin( pinId );
                    m_newLinkPin = nullptr;
                    NodeEditor::Suspend();
                    ImGui::OpenPopup( "Create New Node" );
                    NodeEditor::Resume();
                }
            }
            NodeEditor::EndCreate();
        } else {
            m_newLinkPin = nullptr;
        }

        if ( NodeEditor::BeginDelete() ) {
            NodeEditor::NodeId nodeId = 0;
            while ( NodeEditor::QueryDeletedNode( &nodeId ) ) {
                if ( NodeEditor::AcceptDeletedItem() ) {
                    auto id = std::find_if( m_nodes.begin(), m_nodes.end(), [ nodeId ]( auto &node ) { return node.id == nodeId; } );
                    if ( id != m_nodes.end() ) {
                        m_nodes.erase( id );
                    }
                }
            }
            NodeEditor::LinkId linkId = 0;
            while ( NodeEditor::QueryDeletedLink( &linkId ) ) {
                if ( NodeEditor::AcceptDeletedItem() ) {
                    auto id = std::find_if( m_links.begin(), m_links.end(), [ linkId ]( auto &link ) { return link.id == linkId; } );
                    if ( id != m_links.end() ) {
                        m_links.erase( id );
                    }
                }
            }
            NodeEditor::EndDelete();
        }
    }
}

void GraphEditorWindow::renderPopups( void ) noexcept
{
    NodeEditor::Suspend();
    if ( NodeEditor::ShowNodeContextMenu( &m_contextNodeId ) ) {
        ImGui::OpenPopup( "Node Context Menu" );
    } else if ( NodeEditor::ShowPinContextMenu( &m_contextPinId ) ) {
        ImGui::OpenPopup( "Pin Context Menu" );
    } else if ( NodeEditor::ShowLinkContextMenu( &m_contextLinkId ) ) {
        ImGui::OpenPopup( "Link Context Menu" );
    } else if ( NodeEditor::ShowBackgroundContextMenu() ) {
        ImGui::OpenPopup( "Create New Node" );
        m_newNodeLinkPin = nullptr;
    }
    NodeEditor::Resume();

    NodeEditor::Suspend();
    renderNodeContextMenu();
    renderPinContextMenu();
    renderLinkContextMenu();
    renderCreateNewNodeMenu();
    NodeEditor::Resume();
}

void GraphEditorWindow::renderNodeContextMenu( void ) noexcept
{
    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 8 ) );
    if ( ImGui::BeginPopup( "Node Context Menu" ) ) {
        auto node = findNode( m_contextNodeId );

        ImGui::TextUnformatted( "Node Context Menu" );
        ImGui::Separator();
        if ( node ) {
            ImGui::Text( "ID: %p", node->id.AsPointer() );
            ImGui::Text( "Type: %s", node->type == NodeType::Blueprint ? "Blueprint" : ( node->type == NodeType::Tree ? "Tree" : "Comment" ) );
            ImGui::Text( "Inputs: %d", ( int ) node->inputs.size() );
            ImGui::Text( "Outputs: %d", ( int ) node->outputs.size() );
        } else {
            ImGui::Text( "Unknown node: %p", m_contextNodeId.AsPointer() );
        }
        ImGui::Separator();
        if ( ImGui::MenuItem( "Delete" ) ) {
            NodeEditor::DeleteNode( m_contextNodeId );
        }
        ImGui::EndPopup();
    }
}

void GraphEditorWindow::renderPinContextMenu( void ) noexcept
{
    if ( ImGui::BeginPopup( "Pin Context Menu" ) ) {
        auto pin = findPin( m_contextPinId );

        ImGui::TextUnformatted( "Pin Context Menu" );
        ImGui::Separator();
        if ( pin ) {
            ImGui::Text( "ID: %p", pin->id.AsPointer() );
            if ( pin->node ) {
                ImGui::Text( "Node: %p", pin->node->id.AsPointer() );
            } else {
                ImGui::Text( "Node: %s", "<none>" );
            }
        } else {
            ImGui::Text( "Unknown pin: %p", m_contextPinId.AsPointer() );
        }
        ImGui::EndPopup();
    }
}

void GraphEditorWindow::renderLinkContextMenu( void ) noexcept
{
    if ( ImGui::BeginPopup( "Link Context Menu" ) ) {
        auto link = findLink( m_contextLinkId );

        ImGui::TextUnformatted( "Link Context Menu" );
        ImGui::Separator();
        if ( link ) {
            ImGui::Text( "ID: %p", link->id.AsPointer() );
            ImGui::Text( "From: %p", link->startPinId.AsPointer() );
            ImGui::Text( "To: %p", link->endPinId.AsPointer() );
        } else {
            ImGui::Text( "Unknown link: %p", m_contextLinkId.AsPointer() );
        }
        ImGui::Separator();
        if ( ImGui::MenuItem( "Delete" ) ) {
            NodeEditor::DeleteLink( m_contextLinkId );
        }
        ImGui::EndPopup();
    }
}

void GraphEditorWindow::renderCreateNewNodeMenu( void ) noexcept
{
    auto openPopupPosition = ImGui::GetMousePos();
    if ( ImGui::BeginPopup( "Create New Node" ) ) {
        auto newNodePosition = openPopupPosition;

        GraphNode *node = nullptr;
        if ( ImGui::MenuItem( "Input Action" ) ) {
            node = spawnInputActionNode();
        }
        if ( ImGui::MenuItem( "Output Action" ) ) {
            node = spawnOutputActionNode();
        }
        if ( ImGui::MenuItem( "Branch" ) ) {
            node = spawnBranchNode();
        }
        if ( ImGui::MenuItem( "Do N" ) ) {
            node = spawnDoNNode();
        }
        if ( ImGui::MenuItem( "Set Timer" ) ) {
            node = spawnSetTimerNode();
        }
        if ( ImGui::MenuItem( "Less" ) ) {
            node = spawnLessNode();
        }
        if ( ImGui::MenuItem( "Weird" ) ) {
            node = spawnWeirdNode();
        }
        if ( ImGui::MenuItem( "Trace by Channel" ) ) {
            node = spawnTraceByChannelNode();
        }
        if ( ImGui::MenuItem( "Print String" ) ) {
            node = spawnPrintStringNode();
        }
        ImGui::Separator();
        if ( ImGui::MenuItem( "Comment" ) ) {
            node = spawnCommentNode();
        }
        ImGui::Separator();
        if ( ImGui::MenuItem( "Sequence" ) ) {
            node = spawnTreeSequenceNode();
        }
        if ( ImGui::MenuItem( "Move To" ) ) {
            node = spawnTreeMoveToNode(); // SpawnTreeTaskNode()
        }
        if ( ImGui::MenuItem( "Random Wait" ) ) {
            node = spawnTreeRandomWaitNode(); // SpawnTreeTask2Node()
        }
        ImGui::Separator();
        if ( ImGui::MenuItem( "Message" ) ) {
            node = spawnMessageNode();
        }
        ImGui::Separator();
        if ( ImGui::MenuItem( "Transform" ) ) {
            node = spawnHoudiniTransformNode();
        }
        if ( ImGui::MenuItem( "Group" ) ) {
            node = spawnHoudiniGroupNode();
        }

        if ( node ) {
            buildNodes();

            m_createNewNode = false;

            NodeEditor::SetNodePosition( node->id, newNodePosition );

            if ( auto startPin = m_newNodeLinkPin ) {
                auto &pins = startPin->kind == PinKind::Input ? node->outputs : node->inputs;
                for ( auto &pin : pins ) {
                    if ( canCreateLink( startPin, &pin ) ) {
                        auto endPin = &pin;
                        if ( startPin->kind == PinKind::Input ) {
                            std::swap( startPin, endPin );
                        }
                        m_links.emplace_back( Link( getNextId(), startPin->id, endPin->id ) );
                        m_links.back().color = getIconColor( startPin->type );
                        break;
                    }
                }
            }
        }
        ImGui::EndPopup();
    } else {
        m_createNewNode = false;
    }

    ImGui::PopStyleVar();
}

void GraphEditorWindow::showOrdinals( void ) noexcept
{
    auto editorMin = ImGui::GetItemRectMin();
    auto editorMax = ImGui::GetItemRectMax();

    if ( m_showOrdinals ) {
        int nodeCount = NodeEditor::GetNodeCount();
        std::vector< NodeEditor::NodeId > orderedNodeIds;
        orderedNodeIds.resize( static_cast< size_t >( nodeCount ) );
        NodeEditor::GetOrderedNodeIds( orderedNodeIds.data(), nodeCount );

        auto drawList = ImGui::GetWindowDrawList();
        drawList->PushClipRect( editorMin, editorMax );

        int ordinal = 0;
        for ( auto &nodeId : orderedNodeIds ) {
            auto p0 = NodeEditor::GetNodePosition( nodeId );
            auto p1 = p0 + NodeEditor::GetNodeSize( nodeId );
            p0 = NodeEditor::CanvasToScreen( p0 );
            p1 = NodeEditor::CanvasToScreen( p1 );

            ImGuiTextBuffer builder;
            builder.appendf( "#%d", ordinal++ );

            auto textSize = ImGui::CalcTextSize( builder.c_str() );
            auto padding = ImVec2( 2.0f, 2.0f );
            auto widgetSize = textSize + padding * 2;

            auto widgetPosition = ImVec2( p1.x, p0.y ) + ImVec2( 0, -widgetSize.y );

            drawList->AddRectFilled( widgetPosition, widgetPosition + widgetSize, IM_COL32( 100, 80, 80, 190 ), 3.0f, ImDrawFlags_RoundCornersAll );
            drawList->AddRect( widgetPosition, widgetPosition + widgetSize, IM_COL32( 200, 160, 160, 190 ), 3.0f, ImDrawFlags_RoundCornersAll );
            drawList->AddText( widgetPosition + padding, IM_COL32( 255, 255, 255, 255 ), builder.c_str() );
        }

        drawList->PopClipRect();
    }
}

#endif
