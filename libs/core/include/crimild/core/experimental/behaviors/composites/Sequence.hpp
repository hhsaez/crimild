#ifndef CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_SEQUENCE_
#define CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_SEQUENCE_

#include <crimild/core/experimental/behaviors/composites/Composite.hpp>

namespace crimild::experimental::behaviors::composites {

   class Sequence : public Composite {
   public:
      virtual ~Sequence( void ) noexcept = default;

      virtual State onExecute( void ) override
      {
         // Reevaluate from the first child on every execution, stopping as soon
         // as a child fails or needs more time.
         for ( const auto &b : getBehaviors() ) {
            switch ( b->execute() ) {
               case State::FAILURE:
                  return State::FAILURE;
               case State::RUNNING:
                  return State::RUNNING;
               default:
                  break;
            }
         }
         return State::SUCCESS;
      }
   };

}

#endif
