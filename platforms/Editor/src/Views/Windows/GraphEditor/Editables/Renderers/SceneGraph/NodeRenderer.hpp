#ifndef CRIMILD_EDITOR_EDITABLES_RENDERERS_SCENE_GRAPH_NODE_
#define CRIMILD_EDITOR_EDITABLES_RENDERERS_SCENE_GRAPH_NODE_

#include "Views/Windows/GraphEditor/Editables/Editable.hpp"

namespace crimild::editor::editables {

   class NodeRenderer : public Editable::Renderer {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::editables::NodeRenderer )

   public:
      explicit NodeRenderer( /*GraphEditorContext &ctx*/ ) noexcept
      // : Editable( ctx, name, NodeType::Houdini )
      {
         // getInputs().emplace_back( ctx, this, "", PinType::Flow );
      }

      virtual ~NodeRenderer( void ) noexcept = default;

      virtual void render( GraphEditorContext &ctx, Editable *editable ) override;
   };

}

#endif
