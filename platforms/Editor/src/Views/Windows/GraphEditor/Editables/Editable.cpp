#include "./Editable.hpp"

#include "./Renderers/SceneGraph/GroupRenderer.hpp"
#include "./Renderers/SceneGraph/NodeRenderer.hpp"

using namespace crimild::editor::editables;

std::shared_ptr< Editable > Editable::getOrCreate( std::shared_ptr< Entity > const &entity )
{
   auto editable = entity->getExtension< editables::Editable >();
   if ( editable == nullptr ) {
      std::shared_ptr< Editable::Renderer > renderer = crimild::alloc< NodeRenderer >();
      if ( auto group = dynamic_pointer_cast< crimild::Group >( entity ) ) {
         renderer = crimild::alloc< GroupRenderer >();
      }
      editable = entity->attach< editables::Editable >( renderer );
   }
   return editable;
}

void Editable::render( GraphEditorContext &ctx )
{
   if ( m_renderer != nullptr ) {
      m_renderer->render( ctx, this );
   }

   const auto id = getOwner< crimild::Entity >()->getUniqueID();

   if ( !m_initialized ) {
      ax::NodeEditor::SetNodePosition( id, ImVec2( m_position.x, m_position.y ) );
      m_initialized = true;
   } else {
      ImVec2 newPos = ax::NodeEditor::GetNodePosition( id );
      const auto maybeNewPos = Vector2 { newPos.x, newPos.y };
      if ( maybeNewPos != m_position ) {
         m_position = maybeNewPos;
      }
   }
}

void Editable::renderLinks( GraphEditorContext &ctx )
{
   if ( m_renderer != nullptr ) {
      m_renderer->renderLinks( ctx, this );
   }
}

void Editable::encode( coding::Encoder &encoder )
{
   Extension::encode( encoder );

   encoder.encode( "renderer", m_renderer );
   encoder.encode( "position", m_position );
}

void Editable::decode( coding::Decoder &decoder )
{
   Extension::decode( decoder );

   decoder.decode( "renderer", m_renderer );
   decoder.decode( "position", m_position );
}
