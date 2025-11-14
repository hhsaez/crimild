#include "Signal.hpp"

#include <gtest/gtest.h>

TEST( Signal, construction )
{
   crimild::Signal<> s0;
   crimild::Signal< int > s1;
   crimild::Signal< int, double > s2;
   crimild::Signal< int, std::string, float > s3;

   s0();
   s1( 2 );
   s2( 2, 2.0 );
   s3( 3, "Hello, World!", 42.0f );

   ASSERT_TRUE( true );
}

TEST( Signal, binding_lambda )
{
   crimild::Signal< int > s;
   bool called = false;
   s.bind(
      [ & ]( int value ) {
         called = ( value == 42 );
      }
   );
   s( 42 );
   EXPECT_TRUE( called );
}

TEST( Signal, invocation_order )
{
   std::vector< int > log;
   crimild::Signal< int > s;
   s.bind( [ & ]( int value ) { log.push_back( value + 1 ); } );
   s.bind( [ & ]( int value ) { log.push_back( value + 2 ); } );
   s( 5 );
   ASSERT_EQ( log.size(), 2 );
   ASSERT_EQ( log[ 0 ], 6 );
   ASSERT_EQ( log[ 1 ], 7 );
}

TEST( Signal, bind_to_member_function )
{
   struct Probe {
      void record( int v )
      {
         seen.push_back( v );
      }

      std::vector< int > seen;
   };

   auto probe = std::make_shared< Probe >();

   crimild::Signal< int > s;
   s.bind( probe, &Probe::record );
   s( 9 );

   EXPECT_EQ( probe->seen, std::vector< int > { 9 } );
}

TEST( Signal, bind_to_raw_pointer )
{
   struct Probe {
      void record( int v )
      {
         seen.push_back( v );
      }

      std::vector< int > seen;
   };

   Probe probe;

   crimild::Signal< int > s;
   s.bind( &probe, &Probe::record );

   s( 11 );
   EXPECT_EQ( probe.seen, std::vector< int > { 11 } );
}

TEST( Signal, expired_shared_owners )
{
   struct Probe {
      void record( int v )
      {
         seen.push_back( v );
      }

      std::vector< int > seen;
   };

   auto probe = std::make_shared< Probe >();

   crimild::Signal< int > s;
   s.bind( probe, &Probe::record );
   probe.reset(); // simulate destruction

   EXPECT_EQ( 1, s.size() );
   s( 123 ); // should prune expired connections
   EXPECT_EQ( 0, s.size() );
}

TEST( Signal, unbind_by_id )
{
   crimild::Signal< int > s;
   auto id = s.bind( []( int ) { } );
   EXPECT_TRUE( s.unbind( id ) );
   EXPECT_FALSE( s.unbind( id ) ); // already gone
}

TEST( Signal, unbind_shared_ptr )
{
   struct Probe {
      void record( int v )
      {
         seen.push_back( v );
      }

      std::vector< int > seen;
   };

   crimild::Signal< int > s;
   auto probe = std::make_shared< Probe >();
   s.bind( []( int ) { } );
   s.bind( probe, &Probe::record );
   s.unbind( probe );
   EXPECT_EQ( s.size(), 1 );
}

TEST( Signal, does_not_clear_id_counter )
{
   crimild::Signal< int > s;
   auto first = s.bind( []( int ) { } );
   s.clear();
   auto second = s.bind( []( int ) { } );
   EXPECT_GT( second, first );
   EXPECT_EQ( s.size(), 1 );
}

TEST( Signal, invokes_with_no_connections )
{
   crimild::Signal< int > s;
   s( 10 );
   ASSERT_TRUE( true );
}

TEST( Signal, cleans_up_expired_connections )
{
   struct Probe {
      void record( int v )
      {
         seen.push_back( v );
      }

      std::vector< int > seen;
   };

   crimild::Signal< int > s;
   auto p0 = std::make_shared< Probe >();
   auto p1 = std::make_shared< Probe >();
   s.bind( p0, &Probe::record );
   s.bind( p1, &Probe::record );

   EXPECT_EQ( s.size(), 2 );
   p0.reset();
   s( 10 );
   EXPECT_EQ( s.size(), 1 );
   EXPECT_EQ( p1->seen, std::vector< int > { 10 } );
}

TEST( Signal, reentrant_modifications )
{
   struct Probe {
      void record()
      {
         seen = true;
      }

      bool seen = false;
   };

   crimild::Signal<> s;
   auto p0 = std::make_shared< Probe >();
   auto p1 = std::make_shared< Probe >();
   s.bind( p0, &Probe::record );
   s.bind( [ &s ]() { s.clear(); } );
   s.bind( p1, &Probe::record );

   EXPECT_EQ( s.size(), 3 );
   s();
   EXPECT_EQ( s.size(), 0 );
   EXPECT_TRUE( p0->seen );
   EXPECT_FALSE( p1->seen );

   auto p2 = std::make_shared< Probe >();
   s.bind(
      [ & ]() {
         s.bind( p2, &Probe::record );
      }
   );
   EXPECT_EQ( s.size(), 1 );
   s();
   EXPECT_EQ( s.size(), 2 );
   EXPECT_TRUE( p2->seen );
}

TEST( Signal, reentrant_unbind_skips_pending_handler )
{
   crimild::Signal<> s;
   bool secondCalled = false;
   crimild::Signal<>::ConnectionId secondId;
   s.bind( [ & ] { s.unbind( secondId ); } );
   secondId = s.bind( [ & ] { secondCalled = true; } );
   s();
   EXPECT_FALSE( secondCalled );
}

TEST( Signal, clear_then_bind )
{
   crimild::Signal<> s;
   bool newCalled = false;
   s.bind(
      [ & ] {
         s.clear();
         s.bind( [ & ] { newCalled = true; } );
      }
   );
   s();
   EXPECT_TRUE( newCalled );
}

TEST( Signal, binds_in_order )
{
   crimild::Signal<> s;
   std::vector< int > order;
   s.bind(
      [ & ] {
         s.bind( [ & ] { order.push_back( 2 ); } );
         s.bind( [ & ] { order.push_back( 3 ); } );
         order.push_back( 1 );
      }
   );
   s();
   EXPECT_EQ( order, ( std::vector< int > { 1, 2, 3 } ) );
}
