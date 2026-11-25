#ifndef CRIMILD_BEHAVIORS_BEHAVIOR_
#define CRIMILD_BEHAVIORS_BEHAVIOR_

#include <crimild/core/Entity/Entity.hpp>
#include <crimild/core/experimental/Blackboard.hpp>
#include <crimild/foundation/common/Enableable.hpp>
#include <crimild/foundation/common/Named.hpp>
#include <memory>

namespace crimild::experimental::behaviors {

   namespace composites {

      class Composite;

   }

   namespace decorators {

      class Decorator;

   }

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

      // A behavior has at most one structural parent. The parent owns the child.
      inline bool hasParent( void ) const { return m_parent != nullptr; }
      // Like nodes, retaining a parent requires it to be managed by shared ownership.
      inline std::shared_ptr< Behavior > getParent( void ) const { return retain( m_parent ); }

      // Detach from the current owner, retaining this behavior during the move.
      std::shared_ptr< Behavior > detachFromParent( void );

      inline State getState( void ) const { return m_state; }

      // Advance the current activation, or start a new one after completion.
      // The caller supplies the same non-null board throughout an activation.
      State tick( std::shared_ptr< Blackboard > const &blackboard );

      // Interrupt a running activation; repeated calls have no effect.
      void abort( void );

   protected:
      // Reject invalid ownership before changing a collection or aborting work.
      bool canAttach( const Behavior *child ) const;

      // Called once before an activation's first update.
      // Sets RUNNING. Overrides must call their base implementation first.
      virtual void initialize( std::shared_ptr< Blackboard > const &blackboard );

      // Must return SUCCESS, FAILURE, or RUNNING.
      // READY and ABORTED are managed exclusively by the lifecycle infrastructure.
      virtual State update( std::shared_ptr< Blackboard > const &blackboard );

      // Called once after completion or interruption, with getState() already set
      // to SUCCESS, FAILURE, or ABORTED, matching the supplied state.
      virtual void terminate( State state );

   private:
      friend class composites::Composite;
      friend class decorators::Decorator;

      // Hierarchy owners override this hook; leaves have no children to detach.
      virtual void detach( std::shared_ptr< Behavior > const &behavior );

      inline void setParent( Behavior *parent ) noexcept { m_parent = parent; }

      // Non-owning, including when children are attached during construction.
      Behavior *m_parent = nullptr;
      State m_state = State::READY;
   };

}

#endif
