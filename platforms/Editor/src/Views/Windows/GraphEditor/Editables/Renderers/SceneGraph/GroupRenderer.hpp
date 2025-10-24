#ifndef CRIMILD_EDITOR_EDITABLES_RENDERERS_SCENE_GRAPH_GROUP_
#define CRIMILD_EDITOR_EDITABLES_RENDERERS_SCENE_GRAPH_GROUP_

#include "Views/Windows/GraphEditor/Editables/Editable.hpp"
#include "Views/Windows/GraphEditor/GraphEditorTypes.hpp"

namespace crimild::editor::editables {

   class GroupRenderer : public Editable::Renderer {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::editables::GroupRenderer )

   public:
      GroupRenderer( void ) noexcept = default;
      virtual ~GroupRenderer( void ) noexcept = default;

      virtual void render( GraphEditorContext &ctx, Editable *editable ) override;
      virtual void renderLinks( GraphEditorContext &ctx, Editable *editable ) override;

   private:
      void renderInputs( std::vector< Pin > &inputs );
      void renderOutputs( std::vector< Pin > &outputs );

   private:
      std::unordered_map< UniqueID, GraphLink::Id > m_childLinks;

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
