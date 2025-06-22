#include "AssemblyWorkspace.hpp"

#include "Assemblies/Assembly.hpp"
#include "Foundation/ImGuiUtils.hpp"
#include "Simulation/Project.hpp"
#include "Views/Windows/GraphEditor/GraphEditorWindow.hpp"

using namespace crimild::editor;

namespace ImGui {

   bool Splitter( bool splitVertically, float thickness, float *size1, float *size2, float minSize1, float minSize2, float splitterLongAxisSize = -1.0f ) noexcept
   {
      using namespace ImGui;
      ImGuiContext &g = *GImGui;
      ImGuiWindow *window = g.CurrentWindow;
      ImGuiID id = window->GetID( "##Splitter" );
      ImRect bb;
      bb.Min = window->DC.CursorPos + ( splitVertically ? ImVec2( *size1, 0 ) : ImVec2( 0, *size1 ) );
      bb.Max = bb.Min + CalcItemSize( splitVertically ? ImVec2( thickness, splitterLongAxisSize ) : ImVec2( splitterLongAxisSize, thickness ), 0, 0 );
      return SplitterBehavior( bb, id, splitVertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, minSize1, minSize2, 0 );
   }

}

AssemblyWorkspace::AssemblyWorkspace( std::filesystem::path assemblyPath, std::shared_ptr< Assembly > const &assembly ) noexcept
   : View( assembly->getName() ),
     m_assemblyPath( assemblyPath ),
     m_assembly( assembly )
{
   // no-op
}

void AssemblyWorkspace::draw( void ) noexcept
{
   if ( getSubviews().empty() ) {
      auto graphEditor = crimild::alloc< GraphEditorWindow >();
      auto ctx = crimild::alloc< GraphEditorContext >();
      ctx->setAssembly( m_assembly );
      graphEditor->setContext( ctx );
      addSubview( graphEditor );
   }

   ImGui::BeginChild( ( getUniqueName() + "AssemblyWorkspaceDock" ).c_str(), ImVec2( 0, 0 ), false, ImGuiWindowFlags_NoScrollbar );

   if ( m_assembly != nullptr ) {
      drawContent();
   } else {
      ImGui::Text( "Invalid assembly instance" );
   }

   ImGui::EndChild();
}

namespace ImGui {

   void Splitter( float &leftPanelWidth, float &rightPanelWidth, float splitterWidth )
   {
      float availableWidth = ImGui::GetContentRegionAvail().x;
      rightPanelWidth = availableWidth - leftPanelWidth - splitterWidth;

      // Draw Splitter
      ImGui::SameLine( 0, 0 );
      ImGui::SetCursorPosX( leftPanelWidth );
      ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.5f, 0.25f, 0.5f, 0.5f ) );
      ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImVec4( 0.6f, 0.6f, 0.6f, 0.5f ) );
      ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImVec4( 0.7f, 0.7f, 0.7f, 0.5f ) );

      // Create splitter button
      ImGui::Button( "##splitter", ImVec2( splitterWidth, ImGui::GetContentRegionAvail().y ) );

      // Check for dragging
      if ( ImGui::IsItemActive() ) {
         leftPanelWidth += ImGui::GetIO().MouseDelta.x;
         leftPanelWidth = std::clamp( leftPanelWidth, 100.0f, availableWidth - 100.0f );
      }

      ImGui::PopStyleColor( 3 );
   }

}

void AssemblyWorkspace::drawContent( void ) noexcept
{
   auto graphEditor = getSubviews()[ 0 ];

   static float leftWidth = 300.0f;
   static float rightWidth = 300.0f;

   // Sizes for the inner (vertical) splitter
   static float topHeight = 200.0f;
   static float bottomHeight = 200.0f;

   const float splitterThickness = 5.0f;

   // Outer horizontal splitter
   if ( ImGui::Splitter( true, splitterThickness, &leftWidth, &rightWidth, 100.0f, 100.0f ) ) {
      // handle interaction if needed
   }

   // Left panel (contains a vertical splitter)
   ImGui::BeginChild( "LeftPanel", ImVec2( leftWidth, 0 ), true );
   {
      // Inner vertical splitter
      if ( ImGui::Splitter( false, splitterThickness, &topHeight, &bottomHeight, 100.0f, 100.0f ) ) {
         // handle interaction if needed
      }

      // Top panel
      ImGui::BeginChild( "Scene 3D", ImVec2( 0, topHeight ), true );
      ImGui::Text( "Scene 3D" );
      ImGui::EndChild();

      ImGui::Separator(); // Optional: visual separation

      // Bottom panel
      ImGui::BeginChild( "Notes", ImVec2( 0, 0 ), true );
      ImGui::Text( "Notes" );
      ImGui::EndChild();
   }
   ImGui::EndChild();

   // Right panel
   ImGui::SameLine();
   ImGui::BeginChild( graphEditor->getUniqueName().c_str(), ImVec2( 0, 0 ), true );
   graphEditor->draw();
   ImGui::EndChild();
}

void AssemblyWorkspace::encode( coding::Encoder &encoder ) noexcept
{
   View::encode( encoder );

   encoder.encode( "assemblyPath", m_assemblyPath.string() );

   if ( m_assembly != nullptr ) {
      if ( auto project = Project::getInstance() ) {
         project->save( m_assemblyPath, m_assembly );
      }
   }
}

void AssemblyWorkspace::decode( coding::Decoder &decoder ) noexcept
{
   View::decode( decoder );

   std::string assemblyPath;
   decoder.decode( "assemblyPath", assemblyPath );
   m_assemblyPath = std::filesystem::path( assemblyPath );

   if ( auto project = Project::getInstance() ) {
      m_assembly = project->load< Assembly >( m_assemblyPath );
   }

   if ( m_assembly != nullptr ) {
      auto &subviews = getSubviews();
      for ( auto subview : subviews ) {
         if ( auto graphEditor = dynamic_cast_ptr< GraphEditorWindow >( subview ) ) {
            auto ctx = crimild::alloc< GraphEditorContext >();
            ctx->setAssembly( m_assembly );
            graphEditor->setContext( ctx );
         }
      }
   }
}
