#ifndef CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_SELECTOR_
#define CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_SELECTOR_

#include <crimild/core/experimental/behaviors/composites/Composite.hpp>
#include <cstddef>

namespace crimild::experimental::behaviors::composites {

   // Tries children in order until one succeeds, resuming a running child.
   // Structural edits abort the current activation; the next tick starts fresh.
   class Selector : public Composite {
   public:
      virtual ~Selector( void ) noexcept = default;

   protected:
      virtual void initialize( std::shared_ptr< Blackboard > const &blackboard ) override;
      virtual State update( std::shared_ptr< Blackboard > const &blackboard ) override;
      virtual void terminate( State state ) override;

   private:
      std::vector< std::shared_ptr< Behavior > > m_activeBehaviors;
      std::size_t m_currentBehavior = 0;
   };

}

#endif
