#ifndef CRIMILD_EDITOR_EDITABLE_
#define CRIMILD_EDITOR_EDITABLE_

#include "Views/Windows/GraphEditor/GraphEditorContext.hpp"
#include "Views/Windows/GraphEditor/GraphEditorTypes.hpp"

#include <Crimild_Next.hpp>

namespace crimild::editor::editables {

   // inline unsigned int getNextId( void )
   // {
   //    // TODO: Find a more robust way to assign IDs.
   //    static unsigned int nextId = 0;
   //    return nextId++;
   // }

   // inline unsigned int getNextPinId( void )
   // {
   //    return getNextId();
   // }

   // inline ax::NodeEditor::LinkId getNextLinkId( void ) { return ax::NodeEditor::LinkId( getNextId() ); }

   class Editable;

   struct Pin {
      ax::NodeEditor::PinId id;
      Editable *owner = nullptr;
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

   class Editable : public crimild::next::Object::Extension {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::Editable )

   protected:
      Editable( GraphEditorContext &ctx, std::string_view name, NodeType type ) noexcept
         : m_id( ctx.getNextId() ),
           m_name( name ),
           m_type( type )
      {
         // no-op
      }

      // Only subclasses have write-access to input/output pins.
      [[nodiscard]] inline std::vector< InputPin > &getInputs( void ) { return m_inputs; }
      [[nodiscard]] inline std::vector< OutputPin > &getOutputs( void ) { return m_outputs; }

   public:
      virtual ~Editable( void ) = default;

      [[nodiscard]] inline const std::vector< InputPin > &getInputs( void ) const { return m_inputs; }
      [[nodiscard]] inline const std::vector< OutputPin > &getOutputs( void ) const { return m_outputs; }

   private:
      ax::NodeEditor::NodeId m_id;
      std::string m_name;
      NodeType m_type;
      std::vector< InputPin > m_inputs;
      std::vector< OutputPin > m_outputs;

      ImColor color;
      ImVec2 size;

      std::string state;
      std::string savedState;
   };

}

#endif
