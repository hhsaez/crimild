#include "Views/Windows/GraphEditor/Editables/3D/Node3DEditable.hpp"

using namespace crimild::editor::editables;

namespace NodeEditor = ax::NodeEditor;

void Node3DEditable::render( GraphEditorContext &ctx )
{
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

   NodeEditor::BeginNode( getId() );
   ImGui::BeginVertical( getId().AsPointer() );
   if ( !getInputs().empty() ) {
      ImGui::BeginHorizontal( "inputs" );
      ImGui::Spring( 1, 0 );
      ImRect inputsRect;
      int inputAlpha = 200;
      for ( auto &pin : getInputs() ) {
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

         // if ( m_newLinkPin && !canCreateLink( m_newLinkPin, &pin ) && &pin != m_newLinkPin ) {
         //    inputAlpha = ( int ) ( 255 * ImGui::GetStyle().Alpha * ( 48.0f / 255.0f ) );
         // }
      }
      ImGui::EndHorizontal();
   }
   ImGui::BeginHorizontal( "content_frame" );
   ImGui::Spring( 1, padding );
   ImGui::BeginVertical( "content", ImVec2( 0.0f, 0.0f ) );
   ImGui::Dummy( ImVec2( 160, 0 ) );
   ImGui::Spring( 1 );
   ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0, 0, 0, 1 ) );
   ImGui::TextUnformatted( getName().c_str() );
   ImGui::PopStyleColor();
   ImGui::Spring( 1 );
   ImGui::EndVertical();

   auto contentRect = ImGui_GetItemRect();

   ImGui::Spring( 1, padding );
   ImGui::EndHorizontal();

   if ( !getOutputs().empty() ) {
      ImGui::BeginHorizontal( "outputs" );
      ImGui::Spring( 1, 0 );

      ImRect outputsRect;
      int inputAlpha = 200;
      for ( auto &pin : getOutputs() ) {
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

         // if ( m_newLinkPin && canCreateLink( m_newLinkPin, &pin ) && &pin != m_newLinkPin ) {
         //    inputAlpha = ( int ) ( 255 * ImGui::GetStyle().Alpha * ( 48.0f / 255.0f ) );
         // }
      }
      ImGui::EndHorizontal();
   }
   ImGui::EndVertical();

   NodeEditor::EndNode();
   NodeEditor::PopStyleVar( 7 );
   NodeEditor::PopStyleColor( 4 );
}
