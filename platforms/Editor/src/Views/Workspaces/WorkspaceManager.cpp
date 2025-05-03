#include "WorkspaceManager.hpp"

#include "Foundation/ImGuiUtils.hpp"

using namespace crimild::editor;

void WorkspaceManager::draw( void ) noexcept
{
   ImGuiViewport *viewport = ImGui::GetMainViewport();

   ImGui::SetNextWindowPos( viewport->WorkPos );
   ImGui::SetNextWindowSize( viewport->Size );
   ImGui::SetNextWindowViewport( viewport->ID );

   const auto flags =
      ImGuiWindowFlags_NoTitleBar
      | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_NoResize
      | ImGuiWindowFlags_NoMove
      | ImGuiWindowFlags_NoScrollbar
      | ImGuiWindowFlags_NoBringToFrontOnFocus
      | ImGuiWindowFlags_NoNavFocus
      | ImGuiWindowFlags_NoSavedSettings;

   ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
   ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
   ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );

   ImGui::Begin( "Workspaces", nullptr, flags );

   ImGui::PopStyleVar( 3 );

   static int currentWorkspace = 0;
   std::vector< std::string > workspaces = { "Scene", "Animation" };

   if ( ImGui::BeginTabBar( "WorkspaceTabs" ) ) {
      for ( size_t i = 0; i < workspaces.size(); ++i ) {
         const auto &workspace = workspaces[ i ];
         if ( ImGui::BeginTabItem( workspace.c_str() ) ) {
            currentWorkspace = i;
            ImGui::EndTabItem();
         }
      }
      ImGui::EndTabBar();
   }

   ImGui::BeginChild( "workspaceDock", ImVec2( 0, 0 ), false, ImGuiWindowFlags_NoScrollbar );

   ImGuiID dockspaceID = ImGui::GetID( workspaces[ currentWorkspace ].c_str() );
   ImGui::DockSpace( dockspaceID, ImVec2( 0, 0 ), ImGuiDockNodeFlags_None );

   if ( currentWorkspace == 0 ) {
      ImGui::Begin( "Viewport" );
      ImGui::Text( "Scene Viewport" );
      ImGui::End();

      ImGui::Begin( "Inspector" );
      ImGui::Text( "Scene Properties" );
      ImGui::End();
   } else if ( currentWorkspace == 1 ) {
      ImGui::Begin( "Timeline" );
      ImGui::Text( "Animation Timeline" );
      ImGui::End();

      ImGui::Begin( "Graph" );
      ImGui::Text( "Animation Graph" );
      ImGui::End();
   }

   ImGui::EndChild();

   ImGui::End();
}

void WorkspaceManager::drawContent( void ) noexcept
{
   // TODO
}
