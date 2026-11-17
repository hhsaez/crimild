#include <cassert>
#include <crimild/foundation/common/Invariant.hpp>
#include <crimild/foundation/log/Log.hpp>
#include <cstdio>
#include <cstdlib>

[[noreturn]] void crimild::detail::invariantFailed(
   std::string_view expression,
   std::string_view message,
   std::source_location location
) noexcept
{
   Log::error(
      "Invariant",
      location.file_name(),
      ":",
      location.line(),
      " in ",
      location.function_name(),
      ": invariant '",
      expression,
      "' failed: ",
      message
   );

   // Keep fatal diagnostics visible even when logging is disabled or buffered.
   std::fprintf( stderr, "%s:%u in %s: invariant '", location.file_name(), static_cast< unsigned >( location.line() ), location.function_name() );
   std::fwrite( expression.data(), 1, expression.size(), stderr );
   std::fputs( "' failed: ", stderr );
   std::fwrite( message.data(), 1, message.size(), stderr );
   std::fputc( '\n', stderr );
   std::fflush( stderr );
   assert( false && "Crimild invariant violated" );
   // A debug consumer can call this helper from a library built with NDEBUG.
   std::abort();
}
