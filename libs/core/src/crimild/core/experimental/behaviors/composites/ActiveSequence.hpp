#ifndef CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_ACTIVE_SEQUENCE_
#define CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_ACTIVE_SEQUENCE_

#include <crimild/core/experimental/behaviors/composites/Composite.hpp>

namespace crimild::experimental::behaviors::composites {

   // Rechecks children from the start each tick, continuing only after SUCCESS.
   // A FAILURE or RUNNING result aborts later running children and stops traversal.
   // Nest a remembered Sequence to preserve progress through multiple actions.
   class ActiveSequence : public Composite {
   public:
      virtual ~ActiveSequence( void ) noexcept = default;

   protected:
      virtual State update( Blackboard &blackboard ) override;
   };

}

#endif
