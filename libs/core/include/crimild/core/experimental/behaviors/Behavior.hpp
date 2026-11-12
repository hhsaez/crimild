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
      };

   public:
      virtual ~Behavior( void ) noexcept = default;

      inline State getState( void ) const { return m_state; }

      State tick( void );

      virtual State update( void );

   private:
      State m_state = State::READY;
   };

}

#endif
