#ifndef CRIMILD_BEHAVIORS_BEHAVIOR_
#define CRIMILD_BEHAVIORS_BEHAVIOR_

#include <crimild/core/Entity/Entity.hpp>
#include <crimild/foundation/common/Enableable.hpp>
#include <crimild/foundation/common/Named.hpp>

namespace crimild::experimental::behaviors {

   class Behavior
      : public Entity,
        public Enableable,
        public Named {
   public:
      enum class State {
         READY,
         SUCCESS,
         FAILURE,
         RUNNING,
         ABORTED,
      };

   public:
      virtual ~Behavior( void ) noexcept = default;

      inline State getState( void ) const { return m_state; }

      // Advance the current activation, or start a new one after completion.
      State tick( void );

      // Interrupt a running activation; repeated calls have no effect.
      void abort( void );

   protected:
      // Called once before an activation's first update.
      virtual void initialize( void );

      virtual State update( void );

      // Called after completion or interruption, with the final state stored.
      virtual void terminate( void );

   private:
      State m_state = State::READY;
   };

}

#endif
