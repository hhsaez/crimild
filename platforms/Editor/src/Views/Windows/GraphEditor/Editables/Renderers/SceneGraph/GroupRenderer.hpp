#ifndef CRIMILD_EDITOR_EDITABLES_RENDERERS_SCENE_GRAPH_GROUP_
#define CRIMILD_EDITOR_EDITABLES_RENDERERS_SCENE_GRAPH_GROUP_

#include "Views/Windows/GraphEditor/Editables/Editable.hpp"

namespace crimild::editor::editables {

   class GroupRenderer : public Editable::Renderer {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::editables::GroupRenderer )

   public:
      virtual ~GroupRenderer( void ) noexcept = default;

      virtual void render( GraphEditorContext &ctx, Editable *editable ) override;
   };

}

#endif
