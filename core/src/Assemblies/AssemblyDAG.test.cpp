#include "Common/Signal.hpp"
#include "Entity/Entity.hpp"

#include <gtest/gtest.h>
#include <memory>

namespace crimild::experimental {

   class Assembly;

   /**
    * @brief An entity that is part of an Assambly
    *
    * When to use AssemblyEntity vs standard types?
    * If something needs to cross system boundaries, it becomes an AssemblyEntity
    *
    * Entities in assemblies should always be owned by another entity, with the only
    * exception being the Assembly itself that is allowed to have no owner.
    * Assembly entities are allowed to own other entities, ultimately representing a DAG.
    *
    * Ownership has nothing to do with assembly traversal.
    *
    * Having an setOwner() function instead of a constructor simplifies the definition for
    * derived classes. Otherwise, we will need to declare special constructors everywhere.
    *
    * @todo: There's no references to assemblies here. Can all of this be part
    * of the Entity class instead? Do we want that?
    *
    * @todo: How to enforce owner and/or owned entities to be of a specific type?
    * For example, Events should only own Behavior-type entities.
    * Maybe we can use a template for defining the owned entity type? Like:
    * @code
    * template< class OwnedType >
    * class AssemblyEntity {
    *    std::unordered_set< std::shared_ptr< OwnedType >> m_entities;
      };
    * @endcode
    * One problem is that if an entity supports two or more types, we need to
    * use the most common one (almost always AssemblyEntity), which does not really
    * help.
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
         // Since we're detaching from our owner, keep a retained pointer so we're
         // not destroyed yet.
         auto self = retain( this );
         if ( auto owner = getOwner() ) {
            owner->detach( self );
         }
         return self;
      }

      inline const auto &getOwned( void ) const { return m_ownedEntities; }

      bool owns( std::shared_ptr< AssemblyEntity > const &other ) const
      {
         return getOwned().contains( other );
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

TEST( Assembly, entities )
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
   class Group : public AssemblyEntity { };

   // TODO: How to represent a sorted container?
   // Maybe add a template to AssemblyEntity for storage policy?
   class SortedGroup : public AssemblyEntity { };

   // Unsorted container for entites (same as group?)
   class Bag : public AssemblyEntity { };

   // Resources
   // An entity that can be consumed by other entities.
   class Resource : public AssemblyEntity { };
   class Material : public Resource { };
   class Primitive : public Resource { };

   /**
    * @brief A value that can be shared and/or consumed by many entities
    *
    * If something needs to cross system boundaries, it becomes a Value
    */
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
   class Int32 : public Value< int32_t > { };
   class UInt32 : public Value< uint32_t > { };
   class Float : public Value< float > { };
   class Vector3 : public Value< crimild::Vector3 > { };

   // A node that represents a transformation
   class Node : public AssemblyEntity { };
   class Node3D : public Node { };
   class Node2D : public Node { };
}

namespace crimild::experimental {

   /**
    * @brief An entity representing a 3D object
    */
   class Spatial3D : public Node {
   public:
      Signal<> worldChanged;

   public:
      virtual ~Spatial3D( void ) = default;

      std::shared_ptr< Spatial3D > getParent3D( void ) const
      {
         // TODO: Implement cache for parent 3D
         auto owner = getOwner();
         while ( owner != nullptr ) {
            if ( auto parent = std::dynamic_pointer_cast< Spatial3D >( owner ) ) {
               return parent;
            }
            owner = owner->getOwner();
         }
         return nullptr;
      }

      inline bool isLocalCurrent( void ) const { return m_localIsCurrent; }

      void setLocal( const Transformation &local )
      {
         m_local = local;
         m_localIsCurrent = true;
         m_worldIsCurrent = false;
         worldChanged();
      }

      const Transformation &getLocal( void ) const
      {
         return m_local;
      }

      inline bool isWorldCurrent( void ) const { return m_worldIsCurrent; }

      void setWorld( const Transformation &world )
      {
         m_world = world;
         m_localIsCurrent = false;
         m_worldIsCurrent = true;
         worldChanged();
      }

      const Transformation &getWorld( void ) const
      {
         return m_world;
      }

   private:
      Transformation m_local = Transformation::Constants::IDENTITY;
      bool m_localIsCurrent = true;
      Transformation m_world = Transformation::Constants::IDENTITY;
      bool m_worldIsCurrent = true;
   };

   class Geometry3D : public Spatial3D { };

   /**
    * @brief An entity representing a 2D object
    */
   class Spatial2D : public Node { };

}

TEST( Assembly, spatial )
{
   auto parent = std::make_shared< crimild::experimental::Spatial3D >();
   bool parentWorldChangedCalled = false;
   parent->worldChanged.bind(
      [ &parentWorldChangedCalled ]() {
         parentWorldChangedCalled = true;
      }
   );
   auto child = std::make_shared< crimild::experimental::Spatial3D >();
   parent->attach( child );
   EXPECT_EQ( child->getOwner(), parent );
   EXPECT_EQ( child->getParent3D(), parent );

   auto group = std::make_shared< crimild::experimental::Group >();
   parent->attach( group );
   auto indirectChild = std::make_shared< crimild::experimental::Spatial3D >();
   group->attach( indirectChild );
   EXPECT_EQ( indirectChild->getOwner(), group );
   EXPECT_EQ( indirectChild->getParent3D(), parent );

   // Setting parent local transform invalidates children world transform
   parent->setLocal( crimild::translation( 1, 2, 3 ) );
   EXPECT_TRUE( parentWorldChangedCalled );
   parentWorldChangedCalled = false;
   EXPECT_TRUE( parent->isLocalCurrent() );
   EXPECT_FALSE( parent->isWorldCurrent() );
   EXPECT_TRUE( child->isLocalCurrent() );
   EXPECT_FALSE( child->isWorldCurrent() );
   EXPECT_TRUE( indirectChild->isLocalCurrent() );
   EXPECT_FALSE( indirectChild->isWorldCurrent() );

   // Update parent world only
   parent->getWorld();
   EXPECT_TRUE( parent->isLocalCurrent() );
   EXPECT_TRUE( parent->isWorldCurrent() );
   EXPECT_TRUE( child->isLocalCurrent() );
   EXPECT_FALSE( child->isWorldCurrent() );
   EXPECT_TRUE( indirectChild->isLocalCurrent() );
   EXPECT_FALSE( indirectChild->isWorldCurrent() );

   // Update children world
   child->getWorld();
   indirectChild->getWorld();
   EXPECT_TRUE( child->isLocalCurrent() );
   EXPECT_TRUE( child->isWorldCurrent() );
   EXPECT_TRUE( indirectChild->isLocalCurrent() );
   EXPECT_TRUE( indirectChild->isWorldCurrent() );

   // Setting parent world transform invalidates children world transform
   parent->setWorld( crimild::translation( 1, 2, 3 ) );
   EXPECT_TRUE( parentWorldChangedCalled );
   parentWorldChangedCalled = false;
   EXPECT_FALSE( parent->isLocalCurrent() );
   EXPECT_TRUE( parent->isWorldCurrent() );
   EXPECT_TRUE( child->isLocalCurrent() );
   EXPECT_FALSE( child->isWorldCurrent() );
   EXPECT_TRUE( indirectChild->isLocalCurrent() );
   EXPECT_FALSE( indirectChild->isWorldCurrent() );

   // Getting parent local transform does not invalidate children world transform
   parent->getLocal();
   EXPECT_TRUE( parent->isLocalCurrent() );
   EXPECT_TRUE( parent->isWorldCurrent() );
   EXPECT_TRUE( child->isLocalCurrent() );
   EXPECT_FALSE( child->isWorldCurrent() );
   EXPECT_TRUE( indirectChild->isLocalCurrent() );
   EXPECT_FALSE( indirectChild->isWorldCurrent() );

   // Auto update parent world after changed when child updates
   parent->setLocal( crimild::translation( 1, 2, 3 ) );
   EXPECT_FALSE( parent->isWorldCurrent() );
   EXPECT_FALSE( child->isWorldCurrent() );
   EXPECT_FALSE( indirectChild->isWorldCurrent() );
   indirectChild->getWorld(); // also updates parent's world
   EXPECT_TRUE( parent->isWorldCurrent() );
   EXPECT_FALSE( child->isWorldCurrent() ); // remains unchanged
   EXPECT_TRUE( indirectChild->isWorldCurrent() );

   // TODO: changing parents invalidates world
}

namespace crimild::experimental {

   // An entity that can be executed by other entities.
   class Behavior : public AssemblyEntity { };
   class Composite : public Behavior { };
   class Decorator : public Behavior { };
   class Condition : public Decorator { };
   class Action : public Behavior { };
   class Blackboard : public Resource { };

   // An event occurring during the execution of an Assembly
   class Event : public Entity { };
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
