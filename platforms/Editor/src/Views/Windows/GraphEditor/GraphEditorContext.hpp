#ifndef GRAPH_EDITOR_CONTEXT_
#define GRAPH_EDITOR_CONTEXT_

#include "Assemblies/Assembly.hpp"
#include "Foundation/ImGuiUtils.hpp"

namespace crimild::editor {

   /**
    * @brief Context for editing Assemblies
    *
    * @note (Hernan): My first impulse was to make this a singleton, but that's
    * not a good idea since it is possible to have multiple Assembly graphs active
    * at the same time, each of them with independent contexts.
    */
   class GraphEditorContext {
   public:
      inline unsigned int getNextId( void )
      {
         // TODO: Find a more robust way to assign IDs.
         // This will fail when loading objects previously created, since their IDs will
         // be higher than 0. We need to find a way to persist the last ID used or we can
         // check what is the greatest ID in the current graph and use that as the starting
         // point.
         static unsigned int nextId = 0;
         return nextId++;
      }

      inline unsigned int getNextPinId( void )
      {
         return getNextId();
      }

      inline ax::NodeEditor::LinkId getNextLinkId( void ) { return ax::NodeEditor::LinkId( getNextId() ); }

      inline std::shared_ptr< Assembly > &getAssembly( void ) { return m_assembly; }
      inline void setAssembly( std::shared_ptr< Assembly > const &assembly ) { m_assembly = assembly; }

   private:
      std::shared_ptr< Assembly > m_assembly;
   };

}

#endif
