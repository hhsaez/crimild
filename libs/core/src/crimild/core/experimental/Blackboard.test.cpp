#include <crimild/core/Entity/Entity.hpp>
#include <crimild/core/experimental/Blackboard.hpp>
#include <crimild/core/experimental/nodes/3d/Camera3D.hpp>
#include <crimild/core/experimental/nodes/Node.hpp>
#include <crimild/core/experimental/systems/System.hpp>
#include <gtest/gtest.h>

namespace {

   using crimild::Entity;
   using crimild::Key;
   using crimild::Size2i;
   using crimild::experimental::Blackboard;
   using crimild::experimental::Camera3D;
   using crimild::experimental::Node;

   inline constexpr Key< bool > Visible { "enemy.visible" };
   inline constexpr Key< uint32_t > Count { "enemy.count" };
   inline constexpr Key< float > Distance { "enemy.distance" };
   inline constexpr Key< std::string > Name { "enemy.name" };
   inline constexpr Key< std::shared_ptr< Entity > > Target { "enemy.target" };

}

TEST( experimental_Blackboard, missing_values_return_nullopt )
{
   const Blackboard board;
   EXPECT_FALSE( board.contains( Visible ) );
   EXPECT_FALSE( board.contains( std::string { "enemy.visible" } ) );
   EXPECT_EQ( board.get( Visible ), std::nullopt );
   EXPECT_EQ( board.get< bool >( "enemy.visible" ), std::nullopt );
}

TEST( experimental_Blackboard, stores_each_supported_value_type )
{
   Blackboard board;
   const auto check = [ &board ]< typename T >( Key< T > key, T value ) {
      board.set( key, value );
      EXPECT_EQ( board.get( key ), std::optional< T > { value } );
      EXPECT_EQ( board.get< T >( std::string { key.name } ), std::optional< T > { value } );
   };
   check( Visible, false );
   check( Count, uint32_t { 3 } );
   check( Distance, 4.5f );
   check( Name, std::string { "enemy" } );
   check( Key< Size2i > { "window.size" }, Size2i { 640, 480 } );
   check( Target, std::make_shared< Entity >() );
   check( Key< std::shared_ptr< Node > > { "scene" }, std::make_shared< Node >() );
   check( Key< std::shared_ptr< Camera3D > > { "camera" }, std::make_shared< Camera3D >() );
}

TEST( experimental_Blackboard, typed_and_dynamic_keys_access_the_same_entry )
{
   Blackboard board;
   const std::string prefix = "enemy";
   const std::string dynamicName = prefix + ".count";
   board.set( dynamicName, uint32_t { 2 } );
   EXPECT_EQ( board.get( Count ), 2u );
   board.set( Count, 5u );
   EXPECT_EQ( board.get< uint32_t >( dynamicName ), 5u );
   EXPECT_TRUE( board.contains( dynamicName ) );
   EXPECT_TRUE( board.contains( Count ) );
}

TEST( experimental_Blackboard, names_are_copied_and_compared_by_content )
{
   Blackboard board;
   {
      std::string dynamicName = "enemy.name";
      board.set( dynamicName, std::string { "guard" } );
      dynamicName.assign( "reused storage" );
   }
   board.set( std::string { "enemy.count" }, uint32_t { 2 } );
   EXPECT_EQ( board.get( Name ), "guard" );
   EXPECT_EQ( board.get( Count ), 2u );
   EXPECT_FALSE( board.contains( "reused storage" ) );
}

TEST( experimental_Blackboard, type_mismatch_returns_nullopt_without_numeric_conversion )
{
   Blackboard board;
   board.set( Count, 2u );
   const Key< float > wrongType { Count.name };
   EXPECT_EQ( board.get( wrongType ), std::nullopt );
   EXPECT_EQ( board.get< float >( Count.name ), std::nullopt );
   EXPECT_TRUE( board.contains( wrongType ) );
   EXPECT_EQ( board.get( Count ), 2u );
}

TEST( experimental_Blackboard, replacing_a_name_can_change_its_type )
{
   Blackboard board;
   board.set( Count, 2u );
   board.set( std::string { Count.name }, std::string { "many" } );
   EXPECT_EQ( board.get( Count ), std::nullopt );
   EXPECT_EQ( board.get< std::string >( Count.name ), "many" );
   board.set( Count, 3u );
   EXPECT_EQ( board.get< std::string >( Count.name ), std::nullopt );
   EXPECT_EQ( board.get( Count ), 3u );
}

TEST( experimental_Blackboard, reads_return_copies )
{
   Blackboard board;
   board.set( Name, "guard" );
   auto name = board.get( Name );
   ASSERT_TRUE( name.has_value() );
   name->assign( "changed" );
   EXPECT_EQ( board.get( Name ), "guard" );
}

TEST( experimental_Blackboard, entity_keys_accept_derived_instances_and_keep_them_alive )
{
   Blackboard board;
   std::weak_ptr< Node > lifetime;
   {
      auto node = std::make_shared< Node >();
      lifetime = node;
      board.set( Target, node );
      EXPECT_EQ( board.get( Target ).value(), node );
   }
   EXPECT_FALSE( lifetime.expired() );
   EXPECT_EQ( board.get< std::shared_ptr< Node > >( Target.name ), std::nullopt );
   EXPECT_TRUE( board.erase( Target ) );
   EXPECT_TRUE( lifetime.expired() );
}

TEST( experimental_Blackboard, null_entity_is_distinct_from_a_missing_entry )
{
   Blackboard board;
   board.set( Target, nullptr );
   ASSERT_TRUE( board.get( Target ).has_value() );
   EXPECT_EQ( board.get( Target ).value(), nullptr );
   EXPECT_TRUE( board.contains( Target ) );
   board.erase( Target );
   EXPECT_EQ( board.get( Target ), std::nullopt );
}

TEST( experimental_Blackboard, copies_have_independent_entries_and_shared_entities )
{
   Blackboard original;
   original.set( Count, 1u );
   original.set( Target, std::make_shared< Entity >() );
   auto copy = original;
   copy.set( Count, 2u );
   EXPECT_EQ( original.get( Count ), 1u );
   EXPECT_EQ( copy.get( Count ), 2u );
   EXPECT_EQ( original.get( Target ), copy.get( Target ) );
   copy.clear();
   EXPECT_TRUE( original.contains( Target ) );
}

TEST( experimental_Blackboard, shared_instance_exchanges_a_target_between_callers )
{
   Blackboard board;
   auto target = std::make_shared< Entity >();
   const auto findTarget = [ &target ]( Blackboard &shared ) {
      shared.set( Target, target );
   };
   const auto readTarget = []( const Blackboard &shared ) {
      return shared.get( Target );
   };
   findTarget( board );
   EXPECT_EQ( readTarget( board ), target );
   target = std::make_shared< Entity >();
   findTarget( board );
   EXPECT_EQ( readTarget( board ), target );
}

TEST( experimental_Blackboard, erase_and_clear_remove_entries_using_either_key_form )
{
   Blackboard board;
   board.set( Visible, true );
   board.set( Count, 1u );
   board.set( Name, "guard" );
   EXPECT_TRUE( board.erase( std::string { Visible.name } ) );
   EXPECT_FALSE( board.erase( Visible ) );
   EXPECT_FALSE( board.contains( Visible ) );
   EXPECT_TRUE( board.erase( Count ) );
   EXPECT_FALSE( board.erase( "absent" ) );
   EXPECT_TRUE( board.contains( Name ) );
   board.clear();
   EXPECT_FALSE( board.contains( Name ) );
   EXPECT_EQ( board.get( Name ), std::nullopt );
}

TEST( experimental_Blackboard, existing_system_context_uses_the_same_key_declaration )
{
   using namespace crimild::experimental::systems;
   Context ctx;
   ctx.set( keys::WindowSize, Size2i { 640, 480 } );
   auto copy = ctx;
   copy.set( keys::WindowSize, Size2i { 800, 600 } );
   Blackboard board;
   board.set( keys::WindowSize, ctx.get( keys::WindowSize ).value() );
   EXPECT_EQ( board.get( keys::WindowSize ), ( Size2i { 640, 480 } ) );
   EXPECT_EQ( ctx.get( keys::WindowSize ), ( Size2i { 640, 480 } ) );
   EXPECT_EQ( copy.get( keys::WindowSize ), ( Size2i { 800, 600 } ) );
}
