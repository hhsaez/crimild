#ifndef CRIMILD_EDITOR_VIEWS_WINDOWS_GRAPH_EDITOR_TYPES_
#define CRIMILD_EDITOR_VIEWS_WINDOWS_GRAPH_EDITOR_TYPES_

#include "Foundation/ImGuiUtils.hpp"

namespace crimild::editor {

   enum class PinType {
      Flow,
      Bool,
      Int,
      Float,
      String,
      Object,
      Function,
      Delegate,
   };

   enum class PinKind {
      Output,
      Input,
   };

   enum class NodeType {
      Blueprint,
      Simple,
      Tree,
      Comment,
      Houdini,
   };

   struct GraphNode;

   struct Pin {
      ax::NodeEditor::PinId id;
      GraphNode *node;
      std::string name;
      PinType type;
      PinKind kind;

      Pin( ax::NodeEditor::PinId id, std::string_view name, PinType type )
         : id( id ), node( nullptr ), name( name ), type( type ), kind( PinKind::Input )
      {
         // no-op
      }
   };

   struct GraphNode {
      ax::NodeEditor::NodeId id;
      std::string name;
      std::vector< Pin > inputs;
      std::vector< Pin > outputs;
      ImColor color;
      NodeType type;
      ImVec2 size;

      std::string state;
      std::string savedState;

      GraphNode( ax::NodeEditor::NodeId id, std::string_view name, ImColor color = ImColor( 255, 255, 255 ) )
         : id( id ), name( name ), color( color ), type( NodeType::Blueprint ), size( 0, 0 )
      {
         // no-op
      }
   };

   struct Link {
      ax::NodeEditor::LinkId id;
      ax::NodeEditor::PinId startPinId;
      ax::NodeEditor::PinId endPinId;
      ImColor color;

      Link( ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId, ImColor color = ImColor( 255, 255, 255 ) )
         : id( id ), startPinId( startPinId ), endPinId( endPinId ), color( color )
      {
         // no-op
      }
   };

   struct NodeIdLess {
      bool operator()( const ax::NodeEditor::NodeId &lhs, const ax::NodeEditor::NodeId &rhs ) const
      {
         return lhs.AsPointer() < rhs.AsPointer();
      }
   };

}

#endif
