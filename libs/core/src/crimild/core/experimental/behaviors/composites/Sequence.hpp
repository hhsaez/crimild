#ifndef CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_SEQUENCE_
#define CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_SEQUENCE_

#include <crimild/core/experimental/behaviors/composites/Composite.hpp>
#include <cstddef>

namespace crimild::experimental::behaviors::composites {

   // Runs children in order, resuming the running child on subsequent ticks.
   // Structural edits abort the current activation; the next tick starts fresh.
   class Sequence : public Composite {
   public:
      virtual ~Sequence( void ) noexcept = default;

   protected:
      virtual void initialize( void ) override;
      virtual State update( void ) override;
      virtual void terminate( State state ) override;

   private:
      std::vector< std::shared_ptr< Behavior > > m_activeBehaviors;
      std::size_t m_currentBehavior = 0;
   };

}

#endif
