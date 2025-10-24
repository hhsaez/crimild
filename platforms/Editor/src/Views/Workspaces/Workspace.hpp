#ifndef CRIMILD_EDITOR_VIEWS_WORKSPACES_WORKSPACE_
#define CRIMILD_EDITOR_VIEWS_WORKSPACES_WORKSPACE_

#include "Views/View.hpp"

namespace crimild::editor {

   class Workspace : public View {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::Workspace )

   public:
      Workspace( void ) noexcept = default;

      Workspace( std::string_view name ) noexcept
         : View( name )
      {
         // no-op
      }

      virtual ~Workspace( void ) noexcept = default;

      virtual void draw( void ) noexcept override;

      virtual void drawContent( void ) noexcept override;
   };

}

#endif
