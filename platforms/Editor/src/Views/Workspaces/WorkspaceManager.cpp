#include "Views/Workspaces/WorkspaceManager.hpp"

#include "Foundation/ImGuiUtils.hpp"
#include "Simulation/Editor.hpp"
#include "Simulation/Project.hpp"
#include "Views/Workspaces/Workspace.hpp"

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

   drawContent();

   ImGui::End();
}

void WorkspaceManager::drawContent( void ) noexcept
{
   auto project = Editor::getInstance()->getProject();
   if ( project == nullptr ) {
      return;
   }

   const auto &workspaces = getSubviews();

   ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 10.0f, 10.0f ) );

   if ( ImGui::BeginTabBar( "Workspaces" ) ) {
      for ( auto &workspace : workspaces ) {
         if ( ImGui::BeginTabItem( workspace->getUniqueName().c_str() ) ) {
            workspace->draw();
            ImGui::EndTabItem();
         }
      }
      ImGui::EndTabBar();
   }

   ImGui::PopStyleVar();
}
