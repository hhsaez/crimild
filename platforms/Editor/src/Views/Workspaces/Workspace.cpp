#include "Workspace.hpp"

#include "Foundation/ImGuiUtils.hpp"

using namespace crimild::editor;

void Workspace::draw( void ) noexcept
{
   ImGui::BeginChild( "workspaceDock", ImVec2( 0, 0 ), false, ImGuiWindowFlags_NoScrollbar );

   drawContent();

   ImGui::EndChild();
}

void Workspace::drawContent( void ) noexcept
{
   ImGuiID dockspaceID = ImGui::GetID( getUniqueName().c_str() );
   ImGui::DockSpace( dockspaceID, ImVec2( 0, 0 ), ImGuiDockNodeFlags_None );

   ImGui::Begin( "Viewport" );
   ImGui::Text( "Scene Viewport" );
   ImGui::End();

   ImGui::Begin( "Inspector" );
   ImGui::Text( "Scene Properties" );
   ImGui::End();
}
