#ifndef CRIMILD_EDITOR_EDITABLES_RENDERERS_SCENE_GRAPH_NODE_
#define CRIMILD_EDITOR_EDITABLES_RENDERERS_SCENE_GRAPH_NODE_

#include "Views/Windows/GraphEditor/Editables/Editable.hpp"

namespace crimild::editor::editables {

   class NodeRenderer : public Editable::Renderer {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::editables::NodeRenderer )

   public:
      NodeRenderer( void ) noexcept = default;
      virtual ~NodeRenderer( void ) noexcept = default;

      virtual void render( GraphEditorContext &ctx, Editable *editable ) override;
      virtual void renderLinks( GraphEditorContext &ctx, Editable *editable ) override;

   private:
      bool m_initialized = false;
      Vector2 m_position = Vector2::Constants::ZERO;

      /**
       * @name Coding
       */
      //@{

   public:
      virtual void encode( coding::Encoder &encoder ) override;
      virtual void decode( coding::Decoder &decoder ) override;

      //@}
   };
}

#endif
