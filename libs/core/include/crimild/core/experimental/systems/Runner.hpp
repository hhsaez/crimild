#ifndef CRIMILD_CORE_EXPERIMENTAL_SYSTEMS_RUNNER_
#define CRIMILD_CORE_EXPERIMENTAL_SYSTEMS_RUNNER_

#include <crimild/core/experimental/systems/System.hpp>

namespace crimild::experimental::systems {

   class Runner : public System {
   public:
      Runner( int argc, char **argv ) noexcept;
      virtual ~Runner( void ) noexcept;

   protected:
      virtual System::State onUpdate( void ) override;
   };

}

#endif
