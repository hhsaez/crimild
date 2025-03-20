#ifndef GRAPH_EDITOR_CONTEXT_
#define GRAPH_EDITOR_CONTEXT_

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
         static unsigned int nextId = 0;
         return nextId++; 
      }

      inline unsigned int getNextPinId( void )
      {
         return getNextId();
      }

      inline ax::NodeEditor::LinkId getNextLinkId( void ) { return ax::NodeEditor::LinkId( getNextId() ); }
      
   };

}

#endif

