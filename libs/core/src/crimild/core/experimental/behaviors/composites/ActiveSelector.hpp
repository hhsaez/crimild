#ifndef CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_ACTIVE_SELECTOR_
#define CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_ACTIVE_SELECTOR_

#include <crimild/core/experimental/behaviors/composites/Composite.hpp>

namespace crimild::experimental::behaviors::composites {

   // Reevaluates children in priority order each tick and aborts displaced work.
   class ActiveSelector : public Composite {
   public:
      virtual ~ActiveSelector( void ) noexcept = default;

   protected:
      virtual State update( Blackboard &blackboard ) override;
   };

}

#endif
