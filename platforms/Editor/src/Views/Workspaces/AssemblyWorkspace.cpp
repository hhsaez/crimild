#include "AssemblyWorkspace.hpp"

#include "Assemblies/Assembly.hpp"
#include "Foundation/ImGuiUtils.hpp"
#include "Views/Windows/GraphEditor/GraphEditorWindow.hpp"

using namespace crimild::editor;

AssemblyWorkspace::AssemblyWorkspace( std::filesystem::path path ) noexcept
   : View( path.stem().string() )
{
   if ( std::filesystem::exists( path ) ) {
      // TODO: This needs proper handling for path, starting from the project's root directory
      coding::FileDecoder decoder;
      decoder.read( path );
      if ( decoder.getObjectCount() == 0 ) {
         CRIMILD_LOG_ERROR( "Cannot decode assembly from path ", path );
         return;
      }
      m_assembly = decoder.getObjectAt< Assembly >( 0 );
   } else {
      m_assembly = crimild::alloc< Assembly >( path.string() );
   }

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

   const auto assemblyPath = m_assembly->getName();
   encoder.encode( "assemblyPath", assemblyPath );

   {
      // TODO: This needs proper handling for path, starting from the project's root directory
      coding::FileEncoder encoder;
      encoder.encode( m_assembly );
      if ( !encoder.write( assemblyPath ) ) {
         CRIMILD_LOG_ERROR( "Failed to encode assembly" );
         return;
      }
   }
}

void AssemblyWorkspace::decode( coding::Decoder &decoder ) noexcept
{
   View::decode( decoder );

   std::string assemblyPath;
   decoder.decode( "assemblyPath", assemblyPath );

   {
      // TODO: This needs proper handling for path, starting from the project's root directory
      coding::FileDecoder decoder;
      decoder.read( assemblyPath );
      if ( decoder.getObjectCount() == 0 ) {
         CRIMILD_LOG_ERROR( "Cannot decode assembly from path ", assemblyPath );
         return;
      }
      m_assembly = decoder.getObjectAt< Assembly >( 0 );
   }

   auto &subviews = getSubviews();
   for ( auto subview : subviews ) {
      if ( auto graphEditor = dynamic_cast_ptr< GraphEditorWindow >( subview ) ) {
         auto ctx = crimild::alloc< GraphEditorContext >();
         ctx->setAssembly( m_assembly );
         graphEditor->setContext( ctx );
      }
   }
}
