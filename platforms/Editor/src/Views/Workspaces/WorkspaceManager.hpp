#ifndef CRIMILD_EDITOR_VIEWS_WORKSPACES_WORKSPACE_MANAGER_
#define CRIMILD_EDITOR_VIEWS_WORKSPACES_WORKSPACE_MANAGER_

#include "Views/View.hpp"

namespace crimild::editor {

   class WorkspaceManager : public View {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::WorkspaceManager )

   public:
      WorkspaceManager( void ) noexcept
         : View( "Workspace Manager" )
      {
         // no-op
      }

      ~WorkspaceManager( void ) = default;

      virtual void draw( void ) noexcept override;

      virtual void drawContent( void ) noexcept override;
   };

}

#endif
