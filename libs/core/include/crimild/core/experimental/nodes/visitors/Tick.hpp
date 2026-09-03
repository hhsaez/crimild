#ifndef CRIMILD_CORE_EXPERIMENTAL_NODES_VISITORS_TICK_
#define CRIMILD_CORE_EXPERIMENTAL_NODES_VISITORS_TICK_

#include <crimild/core/Simulation/Clock.hpp>
#include <crimild/core/experimental/nodes/events/OnTick.hpp>
#include <crimild/core/experimental/nodes/visitors/NodeVisitor.hpp>

namespace crimild::experimental {

   class Tick : public crimild::experimental::NodeVoidVisitor {
   public:
      explicit Tick( const crimild::Clock &clock ) noexcept
      {
         m_clock = clock;
      }

      virtual ~Tick( void ) noexcept = default;

      virtual void visitNode( crimild::experimental::Node &node ) override
      {
         if ( !node.isEnabled() ) {
            return;
         }

         if ( auto onTick = dynamic_cast< OnTick * >( &node ) ) {
            onTick->triggered( m_clock );
         }
         crimild::experimental::NodeVoidVisitor::visitNode( node );
      }

   private:
      crimild::Clock m_clock;
   };

}

#endif
