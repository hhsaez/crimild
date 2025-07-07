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

   namespace editables {

      class Editable;

   }

   struct Pin {
      using Id = ax::NodeEditor::PinId;

      Id id;
      editables::Editable *owner = nullptr; // Why not a weak_ptr?
      std::string name;
      PinType type;
      PinKind kind;

      std::function< void( Pin *src, Pin *dst ) > onConnect;
      std::function< void( Pin *src, Pin *dst ) > onDisconnect;
   };

   // enum class NodeType {
   //   Blueprint,
   //   Simple,
   //   Tree,
   //   Comment,
   //   Houdini,
   //};

   // struct GraphNode;

   // struct [[deprecated]] Pin {
   //    ax::NodeEditor::PinId id;
   //    GraphNode *node;
   //    std::string name;
   //    PinType type;
   //    PinKind kind;

   //   Pin( ax::NodeEditor::PinId id, std::string_view name, PinType type )
   //      : id( id ), node( nullptr ), name( name ), type( type ), kind( PinKind::Input )
   //   {
   //      // no-op
   //   }
   //};

   // struct [[deprecated]] GraphNode {
   //    ax::NodeEditor::NodeId id;
   //    std::string name;
   //    std::vector< Pin > inputs;
   //    std::vector< Pin > outputs;
   //    ImColor color;
   //    NodeType type;
   //    ImVec2 size;

   //   std::string state;
   //   std::string savedState;

   //   GraphNode( ax::NodeEditor::NodeId id, std::string_view name, ImColor color = ImColor( 255, 255, 255 ) )
   //      : id( id ), name( name ), color( color ), type( NodeType::Blueprint ), size( 0, 0 )
   //   {
   //      // no-op
   //   }
   //};

   struct GraphLink {
      using Id = size_t;

      Id id;
      Pin::Id startPinId;
      Pin::Id endPinId;
      ImColor color = ImColor( 255, 255, 255 );
      float thickness = 2.0f;

      void render( void ) const
      {
         ax::NodeEditor::Link( id, startPinId, endPinId, color, thickness );
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
