#ifndef CRIMILD_FOUNDATION_COMMON_KEY_
#define CRIMILD_FOUNDATION_COMMON_KEY_

#include <string_view>

namespace crimild {

   /**
    * @brief Typed key for key/value containers
    *
    * The name identifies the entry; its storage must outlive the key.
    */
   template< typename T >
   struct Key {
      std::string_view name;
   };

}

#endif
