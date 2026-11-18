#include <crimild/foundation/common/Macros.hpp>
#include <gtest/gtest.h>

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
TEST( Invariant, failure_reports_the_literal_message )
{
   EXPECT_DEATH_IF_SUPPORTED( CRIMILD_INVARIANT( false, "invariant diagnostic" ), "invariant diagnostic" );
}
#else
TEST( Invariant, disabled_checks_do_not_evaluate_arguments )
{
   int conditions = 0;
   int messages = 0;
   CRIMILD_INVARIANT( ( ++conditions, false ), ( ++messages, "unused" ) );
   EXPECT_EQ( conditions, 0 );
   EXPECT_EQ( messages, 0 );
}
#endif
