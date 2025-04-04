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

void Editable::encode( coding::Encoder &encoder )
{
   Extension::encode( encoder );

   encoder.encode( "renderer", m_renderer );
}

void Editable::decode( coding::Decoder &decoder )
{
    Extension::decode( decoder );

   decoder.decode( "renderer", m_renderer );
}
