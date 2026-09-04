#include "systems/PhysicsSystem.hpp"

#include <crimild/core/experimental/nodes/3d/BoundingVolume3D.hpp>
#include <crimild/core/experimental/nodes/visitors/Select.hpp>
#include <crimild/math/Sphere.hpp>
#include <crimild/math/Transformation.hpp>
#include <crimild/math/intersect.hpp>
#include <crimild/math/radius.hpp>

namespace crimild::experimental {

   void broadPhase( std::shared_ptr< crimild::experimental::Node > const &scene )
   {
      auto bounds = scene->perform< crimild::experimental::Select< BoundingVolume3D > >();
      /*
      for ( auto &b : bounds ) {
         crimild::universal::systems::DebugSystem::addCircle(
            origin( b->getWorld() ),
            radius( b->getWorld() ),
            crimild::Normal3::Constants::UNIT_Z,
            crimild::ColorRGB {
               1.0f,
               0.5f,
               0.0f,
            }
         );
      }
      */

      for ( size_t i = 0; i < bounds.size(); i++ ) {
         for ( size_t j = i + 1; j < bounds.size(); j++ ) {
            float t;
            const bool intersects = crimild::intersect(
               crimild::Sphere {
                  .origin = crimild::origin( bounds[ i ]->getWorld() ),
                  .radius = crimild::radius( bounds[ i ]->getWorld() ),
               },
               crimild::Sphere {
                  .origin = crimild::origin( bounds[ j ]->getWorld() ),
                  .radius = crimild::radius( bounds[ j ]->getWorld() ),
               },
               t
            );
            if ( intersects ) {
               bounds[ i ]->intersectionBegan( *bounds[ i ].get(), *bounds[ j ].get() );
               bounds[ j ]->intersectionBegan( *bounds[ j ].get(), *bounds[ i ].get() );
            }
         }
      }
   }

}

using namespace crimild::experimental::systems;

PhysicsSystem::State PhysicsSystem::onUpdate( const Context &ctx )
{
   if ( auto maybeScene = ctx.get( keys::Scene ) ) {
      if ( auto scene = *maybeScene ) {
         broadPhase( scene );
      } else {
         CRIMILD_LOG_ERROR( "Invalid scene" );
         return State::FAILURE;
      }
   } else {
      CRIMILD_LOG_ERROR( "No scene found" );
      return State::FAILURE;
   }
   return System::onUpdate( ctx );
}
