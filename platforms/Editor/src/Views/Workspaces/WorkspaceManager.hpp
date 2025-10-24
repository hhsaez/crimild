#ifndef CRIMILD_EDITOR_VIEWS_WORKSPACES_WORKSPACE_MANAGER_
#define CRIMILD_EDITOR_VIEWS_WORKSPACES_WORKSPACE_MANAGER_

#include "Views/View.hpp"

namespace crimild::editor {

   class Workspace;

   class WorkspaceManager
      : public View,
        public DynamicSingleton< WorkspaceManager > {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::WorkspaceManager )

   public:
      WorkspaceManager( void ) noexcept
         : View( "Workspace Manager" )
      {
         // no-op
      }

      ~WorkspaceManager( void ) = default;

      template< class WorkspaceType, typename... Args >
      inline std::shared_ptr< WorkspaceType > createWorkspace( Args &&...args ) noexcept
      {
         auto workspace = crimild::alloc< WorkspaceType >( std::forward< Args >( args )... );
         addSubview( workspace );
         return workspace;
      }

      virtual void draw( void ) noexcept override;

      virtual void drawContent( void ) noexcept override;
   };

}

#endif
