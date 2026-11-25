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

TEST( experimental_Blackboard, entity_keys_accept_derived_instances_without_keeping_them_alive )
{
   Blackboard board;
   std::weak_ptr< Node > lifetime;
   {
      auto node = std::make_shared< Node >();
      lifetime = node;
      board.set( Target, node );
      EXPECT_EQ( board.get( Target ).value(), node );
      EXPECT_EQ( board.get< std::shared_ptr< Node > >( Target.name ), std::nullopt );
   }
   EXPECT_TRUE( lifetime.expired() );
   EXPECT_EQ( board.get( Target ), std::nullopt );
   EXPECT_TRUE( board.contains( Target ) );
   EXPECT_TRUE( board.erase( Target ) );
}

TEST( experimental_Blackboard, empty_entity_references_return_nullopt_but_keep_the_name )
{
   Blackboard board;
   board.set( Target, nullptr );
   EXPECT_EQ( board.get( Target ), std::nullopt );
   EXPECT_TRUE( board.contains( Target ) );
   board.erase( Target );
   EXPECT_EQ( board.get( Target ), std::nullopt );
}

TEST( experimental_Blackboard, shared_handles_access_the_same_resource )
{
   const auto original = std::make_shared< Blackboard >();
   original->set( Count, 1u );
   const auto target = std::make_shared< Entity >();
   original->set( Target, target );
   const auto handle = original;
   handle->set( Count, 2u );
   EXPECT_EQ( original->get( Count ), 2u );
   EXPECT_EQ( original->get( Target ), handle->get( Target ) );
   handle->clear();
   EXPECT_FALSE( original->contains( Target ) );
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

TEST( experimental_Blackboard, is_a_resource_that_can_be_exchanged_as_an_entity )
{
   const auto board = std::make_shared< Blackboard >();
   std::shared_ptr< crimild::experimental::Resource > resource = board;
   std::shared_ptr< Entity > entity = resource;
   EXPECT_STREQ( entity->getClassName(), "crimild::experimental::Blackboard" );
   Blackboard other;
   other.set( Target, entity );
   EXPECT_EQ( std::dynamic_pointer_cast< Blackboard >( other.get( Target ).value() ), board );
}

TEST( experimental_Blackboard, shared_and_weak_inputs_use_the_same_weak_storage_for_every_pointer_type )
{
   const auto check = []< typename T >( std::shared_ptr< T > object ) {
      Blackboard board;
      const Key< std::shared_ptr< T > > key { "object" };
      const Key< std::weak_ptr< T > > weakKey { "object" };
      std::weak_ptr< T > weak = object;
      board.set( key, object );
      EXPECT_EQ( board.get( key ), object );
      board.set( key, weak );
      EXPECT_EQ( board.get( key ), object );
      board.set( weakKey, object );
      EXPECT_EQ( board.get( weakKey ), object );
      board.set( weakKey, weak );
      EXPECT_EQ( board.get( weakKey ), object );
      const std::string name = "object";
      board.set( name, object );
      EXPECT_EQ( board.get< std::shared_ptr< T > >( name ), object );
      board.set( name, weak );
      EXPECT_EQ( board.get< std::shared_ptr< T > >( name ), object );
      EXPECT_EQ( board.get< std::weak_ptr< T > >( name ), object );
      object.reset();
      EXPECT_TRUE( weak.expired() );
      EXPECT_EQ( board.get( key ), std::nullopt );
      EXPECT_EQ( board.get( weakKey ), std::nullopt );
      EXPECT_EQ( board.get< std::shared_ptr< T > >( name ), std::nullopt );
      EXPECT_TRUE( board.contains( key ) );
   };
   check( std::make_shared< Entity >() );
   check( std::make_shared< Node >() );
   check( std::make_shared< Camera3D >() );
}

TEST( experimental_Blackboard, weak_derived_inputs_work_with_base_entity_read_keys )
{
   Blackboard board;
   auto node = std::make_shared< Node >();
   board.set( Target, std::weak_ptr< Node > { node } );
   EXPECT_EQ( board.get( Target ), node );
   node.reset();
   EXPECT_EQ( board.get( Target ), std::nullopt );
}

TEST( experimental_Blackboard, a_locked_read_temporarily_owns_the_object )
{
   Blackboard board;
   auto entity = std::make_shared< Entity >();
   std::weak_ptr< Entity > lifetime = entity;
   board.set( Target, entity );
   auto read = board.get( Target );
   ASSERT_TRUE( read.has_value() );
   entity.reset();
   EXPECT_FALSE( lifetime.expired() );
   EXPECT_EQ( board.get( Target ), read );
   read.reset();
   EXPECT_TRUE( lifetime.expired() );
   EXPECT_EQ( board.get( Target ), std::nullopt );
}

TEST( experimental_Blackboard, empty_expired_and_temporary_inputs_do_not_create_an_owner )
{
   Blackboard board;
   board.set( Target, std::weak_ptr< Entity > {} );
   EXPECT_EQ( board.get( Target ), std::nullopt );
   std::weak_ptr< Entity > expired;
   {
      auto entity = std::make_shared< Entity >();
      expired = entity;
   }
   board.set( Target, expired );
   EXPECT_EQ( board.get( Target ), std::nullopt );
   board.set( Target, std::make_shared< Entity >() );
   EXPECT_EQ( board.get( Target ), std::nullopt );
   EXPECT_TRUE( board.contains( Target ) );
}

TEST( experimental_Blackboard, a_board_can_reference_an_owner_without_creating_a_strong_cycle )
{
   struct Owner : Entity {
      std::shared_ptr< Blackboard > board = std::make_shared< Blackboard >();
   };
   std::weak_ptr< Owner > ownerLifetime;
   std::weak_ptr< Blackboard > boardLifetime;
   {
      auto owner = std::make_shared< Owner >();
      ownerLifetime = owner;
      boardLifetime = owner->board;
      owner->board->set( Target, owner );
      EXPECT_EQ( owner->board->get( Target ), owner );
   }
   EXPECT_TRUE( ownerLifetime.expired() );
   EXPECT_TRUE( boardLifetime.expired() );
}
