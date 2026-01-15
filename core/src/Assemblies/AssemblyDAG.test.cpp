#include "Common/Signal.hpp"
#include "Entity/Entity.hpp"

#include <gtest/gtest.h>

namespace crimild::experimental {

   class Assembly;

   /**
    * @brief An entity that is part of an Assambly
    *
    * Entities in assemblies should always be owned by another entity and are allowed to own
    * other entities, ultimately representing a DAG.
    *
    * Ownership has nothing to do with traversal.
    *
    * Having an setOwner() function instead of a constructor simplifies the definition for
    * derived classes. Otherwise, we will need to declare special constructors everywhere.
    */
   class AssemblyEntity : public Entity {
   public:
      /**
       * @brief Indicates that the assembly hierarchy has changed
       *
       * This is used to signal entities that the hierarchy has changed, either because
       * a new owner has been assigned directly to this entity or an ancestor has changed.
       */
      Signal<> ownerChanged;

   protected:
      AssemblyEntity( void ) = default;

   public:
      virtual ~AssemblyEntity( void ) = default;

      inline bool hasOwner( void ) const
      {
         return !m_owner.expired();
      }

      std::shared_ptr< AssemblyEntity > getOwner( void ) const
      {
         return m_owner.lock();
      }

      void attach( std::shared_ptr< AssemblyEntity > const &entity )
      {
         m_ownedEntities.insert( entity );
         entity->setOwner( retain( this ) );
      }

      void detach( std::shared_ptr< AssemblyEntity > const &entity )
      {
         m_ownedEntities.erase( entity );
         entity->setOwner( getOwner() );
      }

      std::shared_ptr< AssemblyEntity > detachFromOwner( void )
      {
         auto self = std::static_pointer_cast< AssemblyEntity >( shared_from_this() );
         if ( auto owner = getOwner() ) {
            owner->detach( self );
         }
         return self;
      }

      bool owns( std::shared_ptr< AssemblyEntity > const &other ) const
      {
         return m_ownedEntities.contains( other );
      }

   private:
      void setOwner( std::shared_ptr< AssemblyEntity > const &owner )
      {
         if ( !m_owner.expired() ) {
            auto owner = m_owner.lock();
            owner->ownerChanged.unbind( retain( this ) );
         }
         m_owner = owner;
         if ( !m_owner.expired() ) {
            m_owner.lock()->ownerChanged.bind( retain( this ), &AssemblyEntity::onOwnerChanged );
         }
         ownerChanged();
      }

      void onOwnerChanged( void )
      {
         ownerChanged();
      }

   private:
      std::weak_ptr< AssemblyEntity > m_owner;
      std::unordered_set< std::shared_ptr< AssemblyEntity > > m_ownedEntities;
   };
}

TEST( AssemblyEntity, test )
{
   class Foo : public crimild::experimental::AssemblyEntity { };

   auto e0 = std::make_shared< Foo >();
   EXPECT_FALSE( e0->hasOwner() );

   auto e1 = std::make_shared< Foo >();
   e0->attach( e1 );
   EXPECT_TRUE( e1->hasOwner() );
   EXPECT_EQ( e1->getOwner(), e0 );
   EXPECT_TRUE( e0->owns( e1 ) );

   auto e2 = std::make_shared< Foo >();
   e0->attach( e2 );

   EXPECT_TRUE( true );
}

TEST( AssemblyEntity, coding )
{
   GTEST_SKIP();
}

namespace crimild::experimental {

   // A container of entities.
   class Group : public Entity { };
   class Bag : public Entity { };

   // Resources
   // An entity that can be consumed by other entities.
   class Resource : public AssemblyEntity { };
   class Material : public Resource { };
   class Primitive : public Resource { };

   template< typename T >
   class Value : public Resource {
   public:
      /**
       * @brief Returns a non-mutable reference
       */
      const T &get( void ) const { return m_value; }

      void set( T value ) { m_value = value; }

   private:
      T m_value;
   };

   class String : public Value< std::string > { };
   class Int : public Value< uint32_t > { };
   class Vector3 : public Value< crimild::Vector3f > { };

   // A node that represents a transformation
   class Node : public AssemblyEntity { };
   class Node3D : public Node { };
   class Node2D : public Node { };
   class Spatial3D : public Node { };
   class Spatial2D : public Node { };

   // An entity that can be triggered as a response to events.
   class Event : public Entity { };

   // An entity that can be executed by other entities.
   class Behavior : public AssemblyEntity { };
   class Composite : public Behavior { };
   class Decorator : public Behavior { };
   class Condition : public Decorator { };
   class Action : public Behavior { };
   class Blackboard : public Resource { };
}

TEST( Assembly, test )
{
   // auto A = crimild::alloc< crimild::next::Assembly >();

   // auto n = A->emplace< crimild::next::Node >();
   // auto s = A->emplace< crimild::next::String >();

   // EXPECT_TRUE( A->contains( n ) );
   // EXPECT_TRUE( A->contains( s ) );

   // EXPECT_EQ( n->getName(), "" );

   // n->setName( "foo" );
   // EXPECT_EQ( n->getName(), "foo" );

   // n->setName( s );
   // s->set( "bar" );
   // EXPECT_EQ( n->getName(), "bar" );

   EXPECT_TRUE( true );
}
