#ifndef CRIMILD_CORE_EXPERIMENTAL_RESOURCE_
#define CRIMILD_CORE_EXPERIMENTAL_RESOURCE_

#include <crimild/core/Entity/Entity.hpp>

namespace crimild::experimental {

   /**
    * @brief An entity shared and consumed by other entities
    */
   class Resource : public Entity {
      CRIMILD_IMPLEMENT_RTTI( crimild::experimental::Resource )

   public:
      virtual ~Resource( void ) noexcept = default;
   };

}

#endif
