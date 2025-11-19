#include "AssemblyWorkspace.hpp"

#include "Foundation/ImGuiUtils.hpp"
#include "Simulation/Project.hpp"
#include "Views/Panels/Scene3D/Scene3DPanel.hpp"
#include "Views/Windows/GraphEditor/GraphEditorWindow.hpp"

using namespace crimild::editor;

namespace ImGui {

   bool Splitter( std::string splitterName, bool splitVertically, float thickness, float *size1, float *size2, float minSize1, float minSize2, float splitterLongAxisSize = -1.0f ) noexcept
   {
      using namespace ImGui;

      const float totalSize = splitVertically ? ImGui::GetContentRegionAvail().x : ImGui::GetContentRegionAvail().y;

      ImGuiContext &g = *GImGui;
      ImGuiWindow *window = g.CurrentWindow;
      ImGuiID id = window->GetID( splitterName.c_str() );
      ImRect bb;
      bb.Min = window->DC.CursorPos + ( splitVertically ? ImVec2( *size1, 0 ) : ImVec2( 0, *size1 ) );
      bb.Max = bb.Min + CalcItemSize( splitVertically ? ImVec2( thickness, splitterLongAxisSize ) : ImVec2( splitterLongAxisSize, thickness ), 0, 0 );
      if ( !SplitterBehavior( bb, id, splitVertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, minSize1, minSize2, 0 ) ) {
         // No dragging.
         return false;
      }

      // Update second constraint after dragging
      *size2 = totalSize - *size1 - thickness;
      return true;
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
      auto scene3D = crimild::alloc< Scene3DPanel >();
      scene3D->setAssembly( m_assembly );
      addSubview( scene3D );

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

void AssemblyWorkspace::drawContent( void ) noexcept
{
   auto scene3D = getSubviews()[ 0 ];
   auto graphEditor = getSubviews()[ 1 ];

   const float totalWidth = ImGui::GetContentRegionAvail().x;
   const float totalHeight = ImGui::GetContentRegionAvail().y;

   const float splitterThickness = 5.0f;

   static float leftWidth = 0.3f * totalWidth;
   static float rightWidth = 300.0f;

   // Sizes for the inner (vertical) splitter
   static float topHeight = leftWidth; // squared size
   static float bottomHeight = 200.0f;

   // Outer horizontal splitter
   if ( ImGui::Splitter( getUniqueName() + "Vertical", true, splitterThickness, &leftWidth, &rightWidth, 50.0f, 50.0f ) ) {
      // Handle dragging if needed
   }

   // Left panel (contains a vertical splitter)
   ImGui::BeginChild( "LeftPanel", ImVec2( leftWidth, 0 ), true );
   {
      // Inner vertical splitter
      if ( ImGui::Splitter( getUniqueName() + "Horizontal", false, splitterThickness, &topHeight, &bottomHeight, 50.0f, 50.0f ) ) {
         // Handle dragging if needed
      }

      // Top panel
      ImGui::BeginChild( scene3D->getUniqueName().c_str(), ImVec2( 0, topHeight ), true );
      scene3D->draw();
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
         } else if ( auto scene3D = dynamic_cast_ptr< Scene3DPanel >( subview ) ) {
            scene3D->setAssembly( m_assembly );
         }
      }
   }
}
