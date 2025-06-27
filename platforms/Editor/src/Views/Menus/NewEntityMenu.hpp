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

         ImGui::Separator();

         if ( ImGui::BeginMenu( "Light" ) ) {
            if ( ImGui::MenuItem( "Directional" ) ) {
               auto light = crimild::alloc< Light >( Light::Type::DIRECTIONAL );
               light->setName( "Directional Light" );
               light->setEnergy( 5 );
               light->setCastShadows( true );
               light->setLocal(
                  lookAt(
                     Point3f { -5, 5, 10 },
                     Point3f { 0, 0, 0 },
                     Vector3::Constants::UP
                  )
               );
               ret = light;
            }
            if ( ImGui::MenuItem( "Point" ) ) {
               auto light = crimild::alloc< Light >( Light::Type::POINT );
               light->setName( "Point Light" );
               light->setLocal( Transformation { .translate = { 0, 1, 0 } } );
               ret = light;
            }
            if ( ImGui::MenuItem( "Spot" ) ) {
               auto light = crimild::alloc< Light >( Light::Type::SPOT );
               light->setName( "Spot Light" );
               light->setColor( ColorRGB { 1.0f, 1.0f, 1.0f } );
               light->setCastShadows( true );
               light->setEnergy( 1000.0f );
               light->setInnerCutoff( Numericf::DEG_TO_RAD * 20.0f );
               light->setOuterCutoff( Numericf::DEG_TO_RAD * 25.0f );
               light->setLocal(
                  lookAt(
                     Point3f { 10, 10, 0 },
                     Point3f { 0, 0, 0 },
                     Vector3::Constants::UP
                  )
               );
               ret = light;
            }

            ImGui::EndMenu();
         }

         return ret;
      }
   };

}

#endif
