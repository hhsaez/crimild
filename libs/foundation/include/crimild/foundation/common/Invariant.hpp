#ifndef CRIMILD_FOUNDATION_INVARIANT_
#define CRIMILD_FOUNDATION_INVARIANT_

#include <source_location>
#include <string_view>

namespace crimild::detail {

   [[noreturn]] void invariantFailed(
      std::string_view expression,
      std::string_view message,
      std::source_location location
   ) noexcept;

}

// Conditions must be side-effect-free: release builds evaluate neither argument.
// In debug builds the condition is evaluated once, and the message only on failure.
#if !defined( NDEBUG )
   #define CRIMILD_INVARIANT( CONDITION, MESSAGE ) \
      do {                                         \
         if ( !( CONDITION ) ) {                   \
            ::crimild::detail::invariantFailed(    \
               #CONDITION,                         \
               ( MESSAGE ),                        \
               std::source_location::current()     \
            );                                     \
         }                                         \
      } while ( false )
#else
   #define CRIMILD_INVARIANT( CONDITION, MESSAGE ) ( ( void ) 0 )
#endif

#endif
