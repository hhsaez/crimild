#include "AssemblyWorkspace.hpp"

#include "Assemblies/Assembly.hpp"
#include "Foundation/ImGuiUtils.hpp"
#include "Views/Windows/GraphEditor/GraphEditorWindow.hpp"

using namespace crimild::editor;

AssemblyWorkspace::AssemblyWorkspace( std::shared_ptr< Assembly > const &assembly ) noexcept
   : View( assembly->getName() ),
     m_assembly( assembly )
{
   crimild::concurrency::sync_frame(
      [ this ] {
         auto graphEditor = crimild::alloc< GraphEditorWindow >();
         auto ctx = crimild::alloc< GraphEditorContext >();
         ctx->setAssembly( m_assembly );
         graphEditor->setContext( ctx );
         addSubview( graphEditor );
      }
   );
}

void AssemblyWorkspace::draw( void ) noexcept
{
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
   ImGuiID dockspaceID = ImGui::GetID( getUniqueName().c_str() );
   ImGui::DockSpace( dockspaceID, ImVec2( 0, 0 ), ImGuiDockNodeFlags_None );

   auto &subviews = getSubviews();
   for ( auto view : subviews ) {
      view->draw();
   }
}

void AssemblyWorkspace::encode( coding::Encoder &encoder ) noexcept
{
   View::encode( encoder );

   encoder.encode( "assembly", m_assembly );
}

void AssemblyWorkspace::decode( coding::Decoder &decoder ) noexcept
{
   View::decode( decoder );

   decoder.decode( "assembly", m_assembly );

   auto &subviews = getSubviews();
   for ( auto subview : subviews ) {
      if ( auto graphEditor = dynamic_cast_ptr< GraphEditorWindow >( subview ) ) {
         auto ctx = crimild::alloc< GraphEditorContext >();
         ctx->setAssembly( m_assembly );
         graphEditor->setContext( ctx );
      }
   }
}
