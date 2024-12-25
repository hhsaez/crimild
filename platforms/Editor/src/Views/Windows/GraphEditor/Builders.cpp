#include "Views/Windows/GraphEditor/Builders.hpp"

using namespace crimild::editor::utils;

namespace NodeEditor = ax::NodeEditor;

BlueprintNodeBuilder::BlueprintNodeBuilder( ImTextureID texture, int textureWidth, int textureHeight ) noexcept
    : m_headerTextureId( texture ),
      m_headerTextureWidth( textureWidth ),
      m_headerTextureHeight( textureHeight )
{
    // no-op
}

void BlueprintNodeBuilder::begin( ax::NodeEditor::NodeId id ) noexcept
{
    m_hasHeader = false;
    m_headerMin = m_headerMax = ImVec2();

    NodeEditor::PushStyleVar( NodeEditor::StyleVar_NodePadding, ImVec4( 8, 4, 8, 8 ) );

    NodeEditor::BeginNode( id );

    ImGui::PushID( id.AsPointer() );
    m_currentNodeId = id;

    setStage( Stage::Begin );
}

void BlueprintNodeBuilder::End( void ) noexcept
{
    setStage( Stage::End );

    NodeEditor::EndNode();

    if ( ImGui::IsItemVisible() ) {
        auto alpha = static_cast< int >( 255 * ImGui::GetStyle().Alpha );
        auto drawList = NodeEditor::GetNodeBackgroundDrawList( m_currentNodeId );
        const auto halfBorderWidth = NodeEditor::GetStyle().NodeBorderWidth * 0.5f;

        auto headerColor = IM_COL32( 0, 0, 0, alpha ) | ( m_headerColor & IM_COL32( 255, 255, 255, 0 ) );
        if ( ( m_headerMax.x > m_headerMin.x ) && ( m_headerMax.y > m_headerMin.y ) && m_headerTextureId ) {
            const auto uv = ImVec2(
                ( m_headerMax.x - m_headerMin.x ) / float( 4.0f * m_headerTextureWidth ),
                ( m_headerMax.y - m_headerMin.y ) / float( 4.0f * m_headerTextureHeight )
            );

            drawList->AddImageRounded(
                m_headerTextureId,
                m_headerMin - ImVec2( 8 - halfBorderWidth, 4 - halfBorderWidth ),
                m_headerMax + ImVec2( 8 - halfBorderWidth, 0 ),
                ImVec2( 0, 0 ),
                uv,
                headerColor,
                NodeEditor::GetStyle().NodeRounding,
#if IMGUI_VERSION_NUM > 18101
                ImDrawFlags_RoundCornersTop
#else
                1 | 2
#endif
            );

            if ( m_contentMin.y > m_headerMax.y ) {
                drawList->AddLine(
                    ImVec2( m_headerMin.x - ( 8 - halfBorderWidth ), m_headerMax.y - 0.5f ),
                    ImVec2( m_headerMax.x + ( 8 - halfBorderWidth ), m_headerMax.y - 0.5f ),
                    ImColor( 255, 255, 255, 96 * alpha / ( 3 * 255 ) ),
                    1.0f
                );
            }
        }
    }

    m_currentNodeId = 0;

    ImGui::PopID();

    NodeEditor::PopStyleVar();

    setStage( Stage::Invalid );
}

void BlueprintNodeBuilder::header( const ImVec4 &color ) noexcept
{
    m_headerColor = ImColor( color );
    setStage( Stage::Header );
}

void BlueprintNodeBuilder::endHeader( void ) noexcept
{
    setStage( Stage::Content );
}

void BlueprintNodeBuilder::input( ax::NodeEditor::PinId id ) noexcept
{
    if ( m_currentStage == Stage::Begin ) {
        setStage( Stage::Content );
    }

    const auto applyPadding = ( m_currentStage == Stage::Input );

    setStage( Stage::Input );

    if ( applyPadding ) {
        ImGui::Spring( 0 );
    }

    pin( id, NodeEditor::PinKind::Input );

    ImGui::BeginHorizontal( id.AsPointer() );
}

void BlueprintNodeBuilder::endInput( void ) noexcept
{
    ImGui::EndHorizontal();
    endPin();
}

void BlueprintNodeBuilder::middle( void ) noexcept
{
    if ( m_currentStage == Stage::Begin ) {
        setStage( Stage::Content );
    }

    setStage( Stage::Middle );
}

void BlueprintNodeBuilder::output( ax::NodeEditor::PinId id ) noexcept
{
    if ( m_currentStage == Stage::Begin ) {
        setStage( Stage::Content );
    }

    const auto applyPadding = ( m_currentStage == Stage::Output );

    setStage( Stage::Output );

    if ( applyPadding ) {
        ImGui::Spring( 0 );
    }

    pin( id, NodeEditor::PinKind::Output );

    ImGui::BeginHorizontal( id.AsPointer() );
}

void BlueprintNodeBuilder::endOutput( void ) noexcept
{
    ImGui::EndHorizontal();
    endPin();
}

bool BlueprintNodeBuilder::setStage( Stage stage ) noexcept
{
    if ( stage == m_currentStage ) {
        return false;
    }

    auto oldStage = m_currentStage;
    m_currentStage = stage;

    ImVec2 cursor;

    switch ( oldStage ) {
        case Stage::Begin:
            break;

        case Stage::Header:
            ImGui::EndHorizontal();
            m_headerMin = ImGui::GetItemRectMin();
            m_headerMax = ImGui::GetItemRectMax();

            // spacing between header and content
            ImGui::Spring( 0, ImGui::GetStyle().ItemSpacing.y * 2.0f );

            break;

        case Stage::Content:
            break;

        case Stage::Input:
            NodeEditor::PopStyleVar( 2 );

            ImGui::Spring( 1, 0 );
            ImGui::EndVertical();

            break;

        case Stage::Middle:
            ImGui::EndVertical();
            break;

        case Stage::Output:
            NodeEditor::PopStyleVar( 2 );

            ImGui::Spring( 1, 0 );
            ImGui::EndVertical();

            break;

        case Stage::End:
            break;

        case Stage::Invalid:
            break;
    }

    switch ( stage ) {
        case Stage::Begin:
            ImGui::BeginVertical( "node" );
            break;

        case Stage::Header:
            m_hasHeader = true;
            ImGui::BeginHorizontal( "header" );
            break;

        case Stage::Content:
            if ( oldStage == Stage::Begin ) {
                ImGui::Spring( 0 );
            }
            ImGui::BeginHorizontal( "content" );
            ImGui::Spring( 0, 0 );
            break;

        case Stage::Input:
            ImGui::BeginVertical( "inputs", ImVec2( 0, 0 ), 0.0f );

            NodeEditor::PushStyleVar( NodeEditor::StyleVar_PivotAlignment, ImVec2( 0, 0.5f ) );
            NodeEditor::PushStyleVar( NodeEditor::StyleVar_PivotSize, ImVec2( 0, 0 ) );

            if ( !m_hasHeader ) {
                ImGui::Spring( 1, 0 );
            }

            break;

        case Stage::Middle:
            ImGui::Spring( 1 );
            ImGui::BeginVertical( "middle", ImVec2( 0, 0 ), 1.0f );
            break;

        case Stage::Output:
            if ( oldStage == Stage::Middle || oldStage == Stage::Input ) {
                ImGui::Spring( 1 );
            } else {
                ImGui::Spring( 0 );
            }

            ImGui::BeginVertical( "outputs", ImVec2( 0, 0 ), 1.0f );

            NodeEditor::PushStyleVar( NodeEditor::StyleVar_PivotAlignment, ImVec2( 1.0f, 0.5f ) );
            NodeEditor::PushStyleVar( NodeEditor::StyleVar_PivotSize, ImVec2( 0, 0 ) );

            if ( !m_hasHeader ) {
                ImGui::Spring( 1, 0 );
            }

            break;

        case Stage::End:
            if ( oldStage == Stage::Input ) {
                ImGui::Spring( 1, 0 );
            }
            if ( oldStage != Stage::Begin ) {
                ImGui::EndHorizontal();
            }

            m_contentMin = ImGui::GetItemRectMin();
            m_contentMax = ImGui::GetItemRectMax();

            ImGui::EndVertical();

            m_nodeMin = ImGui::GetItemRectMin();
            m_nodeMax = ImGui::GetItemRectMax();

            break;

        case Stage::Invalid:
            break;
    }

    return true;
}

void BlueprintNodeBuilder::pin( ax::NodeEditor::PinId id, ax::NodeEditor::PinKind kind ) noexcept
{
    NodeEditor::BeginPin( id, kind );
}

void BlueprintNodeBuilder::endPin( void ) noexcept
{
    NodeEditor::EndPin();
}
