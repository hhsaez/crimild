#ifndef CRIMILD_EDITOR_EDITABLES_3D_NODE3D_
#define CRIMILD_EDITOR_EDITABLES_3D_NODE3D_

#include "Views/Windows/GraphEditor/Editables/Editable.hpp"

namespace crimild::editor::editables {

   class Node3DEditable : public Editable {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::Node3DEditable )

   public:
      explicit Node3DEditable( GraphEditorContext &ctx, std::string_view name = "Node" ) noexcept
         : Editable( ctx, name, NodeType::Houdini )
      {
         getInputs().emplace_back( ctx, this, "", PinType::Flow );
      }

      virtual ~Node3DEditable( void ) noexcept = default;

      virtual void render( GraphEditorContext &ctx ) override;
   };

}

#endif
