#ifndef CRIMILD_EDITOR_VIEWS_MENUS_NEW_ENTITY_
#define CRIMILD_EDITOR_VIEWS_MENUS_NEW_ENTITY_

#include "Foundation/ImGuiUtils.hpp"

#include <Crimild.hpp>

namespace crimild::editor {

   class NewEntityMenu {
   public:
      static std::shared_ptr< Entity > render( void )
      {
         std::shared_ptr< Entity > ret;

         if ( ImGui::MenuItem( "Empty" ) ) {
            ret = crimild::alloc< Group >();
         }

         ImGui::Separator();

         if ( ImGui::BeginMenu( "Geometry" ) ) {
            auto buildGeometry = []( std::string name, auto primitive ) {
               auto geometry = crimild::alloc< Geometry >( name );
               geometry->attachPrimitive( primitive );
               geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::WorldGrid >() );
               geometry->setLocal( Transformation::Constants::IDENTITY );
               return geometry;
            };

            if ( ImGui::MenuItem( "Plane" ) ) {
               ret = buildGeometry( "Plane", QuadPrimitive::UNIT_QUAD );
            }
            if ( ImGui::MenuItem( "Box" ) ) {
               ret = buildGeometry( "Box", BoxPrimitive::UNIT_BOX );
            }
            if ( ImGui::MenuItem( "Sphere" ) ) {
               ret = buildGeometry( "Sphere", SpherePrimitive::UNIT_SPHERE );
            }

            ImGui::EndMenu();
         }

         return ret;
      }
   };

}

#endif
