#ifndef CRIMILD_CORE_NODES_GROUP_
#define CRIMILD_CORE_NODES_GROUP_

#include "Nodes/Node.hpp"
#include "Nodes/mixins.hpp"

namespace crimild::experimental {

   class Group : public WithChildren< Node > {
   public:
      virtual ~Group( void ) = default;

   public:
      void accept( NodeVisitor &visitor ) override;
      void accept( NodeConstVisitor &visitor ) const override;
   };

}

#endif
