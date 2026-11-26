#ifndef CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_PARALLEL_
#define CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_PARALLEL_

#include <crimild/core/experimental/behaviors/composites/Composite.hpp>
#include <cstddef>

namespace crimild::experimental::behaviors::composites {

   // Ticks unfinished children each tick; all must succeed and any failure stops.
   // Structural edits abort the current activation; the next tick starts fresh.
   class Parallel : public Composite {
   public:
      virtual ~Parallel( void ) noexcept = default;

   protected:
      virtual void initialize( void ) override;
      virtual State update( void ) override;
      virtual void terminate( State state ) override;

   private:
      std::vector< std::shared_ptr< Behavior > > m_activeBehaviors;
      std::vector< bool > m_completed;
   };

}

#endif
