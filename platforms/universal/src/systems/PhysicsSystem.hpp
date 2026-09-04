#ifndef CRIMILD_UNIVERSAL_SYSTEMS_PHYSICS_
#define CRIMILD_UNIVERSAL_SYSTEMS_PHYSICS_

#include <crimild/core/experimental/systems/System.hpp>

namespace crimild::experimental::systems {

   class PhysicsSystem : public System {
   public:
      virtual ~PhysicsSystem( void ) noexcept = default;

      virtual State onUpdate( const Context &ctx ) override;
   };

}

#endif
