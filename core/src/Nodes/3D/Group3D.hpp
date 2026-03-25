#ifndef CRIMILD_CORE_NODES_3D_GROUP_
#define CRIMILD_CORE_NODES_3D_GROUP_

#include "Nodes/3D/Spatial3D.hpp"
#include "Nodes/mixins.hpp"

namespace crimild::experimental {

   /**
    * @brief A 3D node with children
    */
   class Group3D : public WithChildren< Spatial3D > {
   public:
      virtual ~Group3D( void ) = default;
   };

}

#endif
