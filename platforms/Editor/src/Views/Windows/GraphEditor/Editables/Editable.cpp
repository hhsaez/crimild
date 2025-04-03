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
