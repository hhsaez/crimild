#ifndef CRIMILD_UNIVERSAL_SYSTEMS_SCENE_RUNNER_
#define CRIMILD_UNIVERSAL_SYSTEMS_SCENE_RUNNER_

#include <crimild/core/experimental/systems/Runner.hpp>

namespace crimild::universal::systems {

   class SceneRunner : public crimild::experimental::systems::Runner {
   public:
      SceneRunner( int argc, char **argv ) noexcept;
      virtual ~SceneRunner( void ) noexcept = default;
   };

}

#endif
