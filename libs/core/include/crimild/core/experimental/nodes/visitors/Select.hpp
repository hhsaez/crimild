#ifndef CRIMILD_CORE_EXPERIMENTAL_NODES_VISITORS_SELECT_
#define CRIMILD_CORE_EXPERIMENTAL_NODES_VISITORS_SELECT_

#include <crimild/core/experimental/nodes/Node.hpp>
#include <crimild/core/experimental/nodes/visitors/NodeVisitor.hpp>

namespace crimild::experimental {

   template< crimild::experimental::IsNode NodeType >
   class Select : public NodeReducerVisitor< std::vector< std::shared_ptr< NodeType > > > {
   public:
      virtual void visitNode( crimild::experimental::Node &node ) override
      {
         if ( !node.isEnabled() ) {
            return;
         }

         if ( auto candidate = dynamic_cast< NodeType * >( &node ) ) {
            this->getResult().push_back( retain( candidate ) );
         }
         NodeReducerVisitor< std::vector< std::shared_ptr< NodeType > > >::visitNode( node );
      }
   };

}

#endif
