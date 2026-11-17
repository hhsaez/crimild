#include <crimild/foundation/common/Invariant.hpp>
#include <crimild/foundation/log/Log.hpp>
#include <gtest/gtest.h>
#include <string>

TEST( Invariant, evaluates_condition_once_only_when_enabled )
{
   int evaluations = 0;
   CRIMILD_INVARIANT( ++evaluations == 1, "condition evaluated once" );
#if !defined( NDEBUG )
   EXPECT_EQ( evaluations, 1 );
#else
   EXPECT_EQ( evaluations, 0 );
#endif
}

TEST( Invariant, successful_checks_do_not_evaluate_the_message )
{
   int messages = 0;
   CRIMILD_INVARIANT( true, ( ++messages, std::string( "unused message" ) ) );
   EXPECT_EQ( messages, 0 );
}

TEST( Invariant, behaves_as_a_single_statement )
{
   bool reachedElse = false;
   if ( false )
      CRIMILD_INVARIANT( false, "unreachable" );
   else
      reachedElse = true;
   EXPECT_TRUE( reachedElse );
}

#if !defined( NDEBUG )
TEST( Invariant, failure_reports_call_site_and_dynamic_message_without_log_handlers )
{
   const std::string message = "dynamic invariant diagnostic";
   EXPECT_DEATH_IF_SUPPORTED(
      {
         crimild::Log::setOutputHandlers( {} );
         CRIMILD_INVARIANT( false, message );
      },
      "Invariant.test.cpp:.*dynamic invariant diagnostic"
   );
}
#else
TEST( Invariant, disabled_checks_do_not_evaluate_failing_conditions_or_messages )
{
   int conditions = 0;
   int messages = 0;
   CRIMILD_INVARIANT( ( ++conditions, false ), ( ++messages, std::string( "unused" ) ) );
   EXPECT_EQ( conditions, 0 );
   EXPECT_EQ( messages, 0 );
}
#endif
