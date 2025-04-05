#ifndef CRIMILD_EDITOR_EDITABLES_RENDERERS_SCENE_GRAPH_GROUP_
#define CRIMILD_EDITOR_EDITABLES_RENDERERS_SCENE_GRAPH_GROUP_

#include "Views/Windows/GraphEditor/Editables/Editable.hpp"

namespace crimild::editor::editables {

   class GroupRenderer : public Editable::Renderer {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::editables::GroupRenderer )

   public:
      GroupRenderer( void ) noexcept = default;
      virtual ~GroupRenderer( void ) noexcept = default;

      virtual void render( GraphEditorContext &ctx, Editable *editable ) override;
      virtual void renderLinks( GraphEditorContext &ctx, Editable *editable ) override;

   private:
      void renderInputs( std::vector< InputPin > &inputs );
      void renderOutputs( std::vector< OutputPin > &outputs );

   private:
      bool m_initialized = false;
      Vector2 m_position = Vector2::Constants::ZERO;

      std::unordered_map< UniqueID, Link > m_links;

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
