#ifndef CRIMILD_CORE_EXPERIMENTAL_NODES_EVENTS_EVENT_
#define CRIMILD_CORE_EXPERIMENTAL_NODES_EVENTS_EVENT_

#include <crimild/core/experimental/behaviors/Behavior.hpp>
#include <crimild/core/experimental/nodes/Node.hpp>
#include <memory>

namespace crimild::experimental {

   class Event;

   template< class T >
   concept IsEvent = std::is_base_of_v< crimild::experimental::Event, T >;

   class Event : public Node {
   public:
      using State = behaviors::Behavior::State;

      virtual ~Event( void ) noexcept = default;

      inline std::shared_ptr< behaviors::Behavior > getBehavior( void ) const { return m_behavior; }
      void setBehavior( std::shared_ptr< behaviors::Behavior > const &behavior );

      inline std::shared_ptr< Blackboard > getBlackboard( void ) const { return m_blackboard; }
      void setBlackboard( std::shared_ptr< Blackboard > const &blackboard );

      // Empty or disabled events are no-ops. A configured behavior requires a board.
      // Each event executes its own behavior instance; boards may be shared.
      virtual State trigger( void );

   private:
      std::shared_ptr< behaviors::Behavior > m_behavior;
      std::shared_ptr< Blackboard > m_blackboard = std::make_shared< Blackboard >();
   };

}

#endif
