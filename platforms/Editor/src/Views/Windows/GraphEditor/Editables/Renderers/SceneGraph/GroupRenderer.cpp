#include "./GroupRenderer.hpp"

using namespace crimild::editor::editables;

namespace NodeEditor = ax::NodeEditor;

void GroupRenderer::render( GraphEditorContext &ctx, Editable *editable )
{
   auto entity = editable->getOwner< crimild::Node >();
   if ( !entity ) {
      return;
   }

   if ( !editable->hasInputPin( "parent" ) ) {
      editable->setInputPin(
         "parent",
         Pin {
            .id = ctx.getNextPinId(),
            .owner = editable,
            .name = "parent",
            .type = PinType::Flow,
            .kind = PinKind::Input,
            .onConnect = [ editable, entity ]( Pin *src, Pin *dst ) {
              // TODO
            },
            .onDisconnect = [ editable, entity ]( Pin *src, Pin *dst ) {
              // TODO 
            },
         }
      );
   }

   if ( !editable->hasOutputPin( "children" ) ) {
      editable->setOutputPin( 
          "children", 
          Pin { 
            .id = ctx.getNextPinId(), 
            .owner = editable, 
            .name = "children", 
            .type = PinType::Flow, 
            .kind = PinKind::Output,
            .onConnect = [ editable, entity ]( Pin *src, Pin *dst ) {
              CRIMILD_LOG_DEBUG( "Connected" );
              auto group = static_pointer_cast< Group >( src->owner->getOwner() );
              auto node = static_pointer_cast< Node >( dst->owner->getOwner() );
              group->attachNode( node );
            },
            .onDisconnect = [ editable, entity ]( Pin *src, Pin *dst ) {
              CRIMILD_LOG_DEBUG( "Disconnected" );
            },
         }
      );
   }

   const float rounding = 10.0f;
   const float padding = 12.0f;

   NodeEditor::PushStyleColor( NodeEditor::StyleColor_NodeBg, ImColor( 229, 229, 129, 200 ) );
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

   NodeEditor::NodeId id = entity->getUniqueID();

   std::vector< Pin > inputs { editable->getInputPin( "parent" ) };
   std::vector< Pin > outputs { editable->getOutputPin( "children" ) };

   NodeEditor::BeginNode( id );
   ImGui::BeginVertical( id.AsPointer() );

   renderInputs( inputs );

   ImGui::BeginHorizontal( "content_frame" );
   ImGui::Spring( 1, padding );
   ImGui::BeginVertical( "content", ImVec2( 0.0f, 0.0f ) );
   ImGui::Dummy( ImVec2( 160, 0 ) );
   ImGui::Spring( 1 );
   ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0, 0, 0, 1 ) );
   ImGui::TextUnformatted( !entity->getName().empty() ? entity->getName().c_str() : entity->getClassName() );
   ImGui::PopStyleColor();
   ImGui::Spring( 1 );
   ImGui::EndVertical();

   auto contentRect = ImGui_GetItemRect();

   ImGui::Spring( 1, padding );
   ImGui::EndHorizontal();

   renderOutputs( outputs );

   ImGui::EndVertical();

   NodeEditor::EndNode();
   NodeEditor::PopStyleVar( 7 );
   NodeEditor::PopStyleColor( 4 );
}

void GroupRenderer::renderInputs( std::vector< Pin > &inputs )
{
   if ( inputs.empty() ) {
      return;
   }

   const float padding = 12.0f;
   const auto pinBackground = NodeEditor::GetStyle().Colors[ NodeEditor::StyleColor_NodeBg ];

   ImGui::BeginHorizontal( "inputs" );
   ImGui::Spring( 1, 0 );
   ImRect inputsRect;
   int inputAlpha = 200;
   for ( auto &pin : inputs ) {
      ImGui::Dummy( ImVec2( padding, padding ) );
      inputsRect = ImGui_GetItemRect();
      ImGui::Spring( 1, 0 );
      inputsRect.Min.y -= padding;
      inputsRect.Max.y -= padding;

      NodeEditor::PushStyleVar( NodeEditor::StyleVar_PinCorners, ImDrawFlags_RoundCornersAll );
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
         ImDrawFlags_RoundCornersAll
      );
      drawList->AddRect(
         inputsRect.GetTL(),
         inputsRect.GetBR(),
         IM_COL32( ( int ) ( 255 * pinBackground.x ), ( int ) ( 255 * pinBackground.y ), ( int ) ( 255 * pinBackground.z ), inputAlpha ),
         4.0f,
         ImDrawFlags_RoundCornersAll
      );

      // if ( m_newLinkPin && !canCreateLink( m_newLinkPin, &pin ) && &pin != m_newLinkPin ) {
      //    inputAlpha = ( int ) ( 255 * ImGui::GetStyle().Alpha * ( 48.0f / 255.0f ) );
      // }
   }
   ImGui::EndHorizontal();
}

void GroupRenderer::renderOutputs( std::vector< Pin > &outputs )
{
   if ( outputs.empty() ) {
      return;
   }

   const float padding = 12.0f;
   const auto pinBackground = NodeEditor::GetStyle().Colors[ NodeEditor::StyleColor_NodeBg ];

   ImGui::BeginHorizontal( "outputs" );
   ImGui::Spring( 1, 0 );

   ImRect outputsRect;
   int inputAlpha = 200;
   for ( auto &pin : outputs ) {
      ImGui::Dummy( ImVec2( padding, padding ) );
      outputsRect = ImGui_GetItemRect();
      ImGui::Spring( 1, 0 );
      outputsRect.Min.y += padding;
      outputsRect.Max.y += padding;

      NodeEditor::PushStyleVar( NodeEditor::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop );
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
         ImDrawFlags_RoundCornersAll
      );
      drawList->AddRect(
         outputsRect.GetTL(),
         outputsRect.GetBR(),
         IM_COL32( ( int ) ( 255 * pinBackground.x ), ( int ) ( 255 * pinBackground.y ), ( int ) ( 255 * pinBackground.z ), inputAlpha ),
         4.0f,
         ImDrawFlags_RoundCornersAll
      );

      // if ( m_newLinkPin && canCreateLink( m_newLinkPin, &pin ) && &pin != m_newLinkPin ) {
      //    inputAlpha = ( int ) ( 255 * ImGui::GetStyle().Alpha * ( 48.0f / 255.0f ) );
      // }
   }
   ImGui::EndHorizontal();
}

void GroupRenderer::renderLinks( GraphEditorContext &ctx, Editable *editable )
{
   auto entity = editable->getOwner< crimild::Group >();

   if ( !editable->hasOutputPin( "children" ) ) {
      return;
   }

   auto &childrenPin = editable->getOutputPin( "children" );

   entity->forEachNode(
      [ & ]( auto child ) {
         if ( child != nullptr ) {
            if ( !m_links.contains( child->getUniqueID() ) ) {
               if ( auto editable = child->getExtension< Editable >() ) {
                  if ( !editable->hasInputPin( "parent" ) ) {
                     return;
                  }
                  auto &childPin = editable->getInputPin( "parent" );
                  m_links[ child->getUniqueID() ] = Link( ctx.getNextLinkId(), childrenPin.id, childPin.id );
               } else {
                  return;
               }
            }
            auto &link = m_links.at( child->getUniqueID() );
            NodeEditor::Link( link.id, link.startPinId, link.endPinId, link.color, 2.0f );
         }
      }
   );
}

void GroupRenderer::encode( coding::Encoder &encoder )
{
   Entity::encode( encoder );
}

void GroupRenderer::decode( coding::Decoder &decoder )
{
   Entity::decode( decoder );
}
