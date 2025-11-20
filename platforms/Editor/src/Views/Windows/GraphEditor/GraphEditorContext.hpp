#ifndef GRAPH_EDITOR_CONTEXT_
#define GRAPH_EDITOR_CONTEXT_

#include "Foundation/ImGuiUtils.hpp"
#include "Views/Windows/GraphEditor/GraphEditorTypes.hpp"

#include <unordered_map>

namespace crimild {

   class Assembly;

}

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
      inline ax::NodeEditor::NodeId getNextNodeId( void )
      {
         return getNextId();
      }

      inline ax::NodeEditor::PinId getNextPinId( void )
      {
         return getNextId();
      }

      inline std::shared_ptr< Assembly > &getAssembly( void ) { return m_assembly; }
      inline void setAssembly( std::shared_ptr< Assembly > const &assembly ) { m_assembly = assembly; }

      std::shared_ptr< GraphLink > createLink( void )
      {
         auto link = crimild::alloc< GraphLink >();
         link->id = getNextLinkId();
         m_links[ link->id ] = link;
         return link;
      }

      void destroyLink( GraphLink::Id linkId )
      {
         if ( m_links.contains( linkId ) ) {
            m_links.erase( linkId );
         }
      }

      std::shared_ptr< GraphLink > getLink( GraphLink::Id linkId )
      {
         return m_links.contains( linkId ) ? m_links.at( linkId ) : nullptr;
      }

   private:
      inline size_t getNextId( void )
      {
         // TODO: Find a more robust way to assign IDs.
         // This will fail when loading objects previously created, since their IDs will
         // be higher than 0. We need to find a way to persist the last ID used or we can
         // check what is the greatest ID in the current graph and use that as the starting
         // point.
         static size_t nextId = 0;
         return nextId++;
      }

      inline GraphLink::Id getNextLinkId( void )
      {
         return getNextId();
      }

   private:
      std::shared_ptr< Assembly > m_assembly;

      std::unordered_map< GraphLink::Id, std::shared_ptr< GraphLink > > m_links;
   };

}

#endif
