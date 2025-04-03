#ifndef CRIMILD_EDITOR_EDITABLE_
#define CRIMILD_EDITOR_EDITABLE_

#include "Views/Windows/GraphEditor/GraphEditorContext.hpp"
#include "Views/Windows/GraphEditor/GraphEditorTypes.hpp"

#include <Crimild.hpp>

namespace crimild::editor::editables {

   class Editable;

   struct Pin {
      ax::NodeEditor::PinId id;
      Editable *owner = nullptr; // Why not a weak_ptr?
      std::string name;
      PinType type;
      PinKind kind;

      Pin( GraphEditorContext &ctx, Editable *owner, std::string_view name, PinType type, PinKind kind )
         : id( ctx.getNextPinId() ),
           owner( owner ),
           name( name ),
           type( type ),
           kind( kind )
      {
         // no-op
      }
   };

   struct InputPin : public Pin {
      InputPin( GraphEditorContext &ctx, Editable *owner, std::string_view name, PinType type )
         : Pin( ctx, owner, name, type, PinKind::Input )
      {
         // no-op
      }
   };

   struct OutputPin : public Pin {
      OutputPin( GraphEditorContext &ctx, Editable *owner, std::string_view name, PinType type )
         : Pin( ctx, owner, name, type, PinKind::Output )
      {
         // no-op
      }
   };

   class Editable : public crimild::Extension {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::Editable )

   public:
      static std::shared_ptr< Editable > getOrCreate( std::shared_ptr< Entity > const &entity );

   public:
      class Renderer : public crimild::Entity {
      public:
         virtual ~Renderer( void ) noexcept = default;

         virtual void render( GraphEditorContext &ctx, Editable *editable ) = 0;

         [[nodiscard]] inline const std::vector< InputPin > &getInputs( void ) const { return m_inputs; }
         [[nodiscard]] inline const std::vector< OutputPin > &getOutputs( void ) const { return m_outputs; }

      private:
         std::vector< InputPin > m_inputs;
         std::vector< OutputPin > m_outputs;
      };

   protected:
      // // Only subclasses have write-access to input/output pins.
      // [[nodiscard]] inline std::vector< InputPin > &getInputs( void ) { return m_inputs; }
      // [[nodiscard]] inline std::vector< OutputPin > &getOutputs( void ) { return m_outputs; }

   public:
      explicit Editable( std::shared_ptr< Renderer > const &renderer ) noexcept
         : m_renderer( renderer )
      {
         // no-op
      }

      virtual ~Editable( void ) = default;

      // inline ax::NodeEditor::NodeId getId( void ) const { return m_id; }

      // inline const std::string &getName( void ) const { return m_name; }

      inline void render( GraphEditorContext &ctx ) { m_renderer->render( ctx, this ); }

   private:
      // ax::NodeEditor::NodeId m_id;
      std::shared_ptr< Renderer > m_renderer;
      // std::string m_name;
      // NodeType m_type;

      // ImColor color;
      // ImVec2 size;

      // std::string state;
      // std::string savedState;
   };

}

#endif
