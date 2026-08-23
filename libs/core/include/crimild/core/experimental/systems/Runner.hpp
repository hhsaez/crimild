#ifndef CRIMILD_CORE_EXPERIMENTAL_SYSTEMS_RUNNER_
#define CRIMILD_CORE_EXPERIMENTAL_SYSTEMS_RUNNER_

#include <crimild/core/experimental/behaviors/decorators/Decorator.hpp>

namespace crimild::experimental::systems {

   class Runner : public crimild::experimental::behaviors::decorators::Decorator {
   public:
      Runner( int argc, char **argv ) noexcept;
      virtual ~Runner( void ) noexcept;

      virtual State onExecute( void ) override;
   };

}

#endif
