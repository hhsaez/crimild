#ifndef CRIMILD_CORE_EXPERIMENTAL_NODES_EVENTS_EVENT_
#define CRIMILD_CORE_EXPERIMENTAL_NODES_EVENTS_EVENT_

#include <crimild/core/experimental/nodes/Node.hpp>

namespace crimild::experimental {

   class Event;

   template< class T >
   concept IsEvent = std::is_base_of_v< crimild::experimental::Event, T >;

   class Event : public Node {
   public:
      virtual ~Event( void ) noexcept = default;
   };

}

#endif
