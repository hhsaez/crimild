#ifndef CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_MONITOR_
#define CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_MONITOR_

#include <crimild/core/experimental/behaviors/composites/Composite.hpp>

namespace crimild::experimental::behaviors::composites {

   // Exactly two children: a condition subtree, followed by an action subtree.
   // Rechecks the condition each tick: SUCCESS or RUNNING permits the action.
   // Either failure stops the monitor; only the action can complete it successfully.
   // Termination aborts remaining work, including the condition if still running.
   class Monitor : public Composite {
   public:
      virtual ~Monitor( void ) noexcept = default;

   protected:
      virtual State update( void ) override;
   };

}

#endif
