#include "Assembly.hpp"

#include "Entity/Entity.hpp"

#include <gtest/gtest.h>
#include <string>

TEST( Assembly, construction )
{
   auto a0 = crimild::alloc< crimild::Assembly >();
   auto a1 = crimild::alloc< crimild::Assembly >( "foo" );
   EXPECT_EQ( a1->getName(), "foo" );
}

TEST( Assembly, entity_management )
{
   auto e = crimild::alloc< crimild::Entity >();
   auto a = crimild::Assembly();
   a.addEntity( e );
   EXPECT_EQ( a.getEntities().size(), 1 );

   a.removeEntity( e );
   EXPECT_TRUE( a.getEntities().empty() );
}

TEST( Assembly, avoid_adding_duplicate_entities )
{
   auto e = crimild::alloc< crimild::Entity >();

   auto a = crimild::alloc< crimild::Assembly >();
   a->addEntity( e );
   EXPECT_EQ( a->getEntities().size(), 1 );
   a->addEntity( e );
   EXPECT_EQ( a->getEntities().size(), 1 );
}

TEST( Assembly, remove_missing_noop )
{
   auto e1 = crimild::alloc< crimild::Entity >();
   auto e2 = crimild::alloc< crimild::Entity >();

   auto a = crimild::alloc< crimild::Assembly >();
   a->addEntity( e1 );
   a->removeEntity( e2 );
   EXPECT_EQ( a->getEntities().size(), 1 );
}

TEST( Assembly, RTTI )
{
   EXPECT_STREQ( crimild::Assembly::__CLASS_NAME, "crimild::Assembly" );
}

namespace crimild {

   using GraphNodeId = size_t;

   struct GraphEdge {
      GraphNodeId src;
      GraphNodeId dst;
      float weight;
   };

   template< typename T >
   struct GraphNode {
      GraphNodeId id;

      T value;

      // How can we represent a many-to-many relationships?
      // Parallel edges
      std::unordered_map< uint64_t, GraphEdge > edges;

      size_t getEdgeCount( void ) const { return edges.size(); }

      std::optional< GraphEdge > getEdge( GraphNodeId n ) const
      {
         if ( edges.contains( n ) ) {
            return edges.at( n );
         }
         return {};
      }

      void addEdge( GraphNodeId n, float weight )
      {
         edges[ n ] = GraphEdge { id, n, weight };
      }

      void removeEdge( GraphNodeId n )
      {
         edges.erase( n );
      }

      std::vector< GraphEdge > getEdges( void ) const
      {
         std::vector< GraphEdge > ret;
         for ( auto &it : edges ) {
            ret.push_back( it.second );
         }
         return ret;
      }

      std::vector< GraphEdge > getSortedEdges( void ) const
      {
         auto ret = getEdges();
         std::sort(
            ret.begin(),
            ret.end(),
            []( auto &a, auto &b ) {
               return a.dst < b.dst;
            }
         );
         return ret;
      }

      std::vector< GraphNodeId > getNeighbors( void ) const
      {
         std::vector< GraphNodeId > ret;
         for ( const auto &e : edges ) {
            ret.push_back( e.first );
         }
         return ret;
      }
   };

   template< typename T, bool GRAPH_IS_DIRECTED = true >
   struct Graph {

      GraphNodeId addNode( T value )
      {
         const auto id = m_nextNodeId++;
         m_nodes[ id ] = GraphNode { id, value };
         return id;
      }

      bool hasEdge( GraphNodeId src, GraphNodeId dst ) const
      {
         return getEdge( src, dst ).has_value();
      }

      std::optional< GraphEdge > getEdge( GraphNodeId src, GraphNodeId dst ) const
      {
         if ( !m_nodes.contains( src ) || !m_nodes.contains( dst ) ) {
            return {};
         }
         return m_nodes.at( src ).getEdge( dst );
      }

      std::vector< GraphEdge > getEdges( void ) const
      {
         std::vector< GraphEdge > ret;
         for ( const auto &it : m_nodes ) {
            const auto edges = it.second->getEdges();
            for ( const auto &e : edges ) {
               ret.push_back( e );
            }
         }
         return ret;
      }

      bool addEdge( GraphNodeId src, GraphNodeId dst, float weight )
      {
         if ( !m_nodes.contains( src ) || !m_nodes.contains( dst ) ) {
            // TODO: add error logging
            return false;
         }
         m_nodes[ src ].addEdge( dst, weight );
         if ( !GRAPH_IS_DIRECTED ) {
            m_nodes[ dst ].addEdge( src, weight );
         }
         return true;
      }

      bool removeEdge( GraphNodeId src, GraphNodeId dst )
      {
         if ( !m_nodes.contains( src ) || !m_nodes.contains( dst ) ) {
            // TODO: add error logging
            return false;
         }
         m_nodes[ src ].removeEdge( dst );
         if ( !GRAPH_IS_DIRECTED ) {
            m_nodes[ dst ].removeEdge( src );
         }
         return true;
      }

   private:
      size_t m_nextNodeId = 0;
      std::unordered_map< GraphNodeId, GraphNode< T > > m_nodes;
   };

}

TEST( Graph, test )
{
   crimild::Graph< int > g;
   EXPECT_EQ( 0, g.addNode( 0 ) );
   EXPECT_EQ( 1, g.addNode( 1 ) );
   EXPECT_EQ( 2, g.addNode( 2 ) );
   EXPECT_EQ( 3, g.addNode( 3 ) );
   EXPECT_EQ( 4, g.addNode( 4 ) );
   EXPECT_TRUE( g.addEdge( 0, 1, 1.0f ) );
   EXPECT_TRUE( g.addEdge( 0, 3, 1.0f ) );
   EXPECT_TRUE( g.addEdge( 0, 4, 3.0f ) );
   EXPECT_TRUE( g.addEdge( 1, 2, 2.0f ) );
   EXPECT_TRUE( g.addEdge( 1, 4, 1.0f ) );
   EXPECT_TRUE( g.addEdge( 3, 4, 3.0f ) );
   EXPECT_TRUE( g.addEdge( 4, 2, 3.0f ) );
   EXPECT_TRUE( g.addEdge( 4, 3, 3.0f ) );
}

namespace crimild::next {

   class Assembly;

   /**
    * @brief An entity that is part of an Assambly
    *
    * Having an setAssembly() function instead of a constructor simplifies the definition for
    * derived classes. Otherwise, we will need to declare special constructors everywhere.
    */
   class AssemblyEntity : public Entity {
   public:
      using ID = coding::Codable::UniqueID;

      // Having a type of entity could help speed up some search algorithms in the graph.
      // For example, when looking for parent transformations, we only need to search
      // for entities of type "Node" or "Container"
      enum class Type {
         NODE,
         CONTAINER,
         RESOURCE,
         EVENT,
         BEHAVIOR,
      };

   public:
      virtual ~AssemblyEntity( void ) = default;

      inline void setAssembly( std::shared_ptr< Assembly > const &assembly ) { m_assembly = assembly; }
      inline std::shared_ptr< Assembly > getAssembly( void ) const { return !m_assembly.expired() ? m_assembly.lock() : nullptr; }

   private:
      // Owner of this entity
      std::weak_ptr< Assembly > m_assembly;
   };

   /**
    * @brief A graph of entities
    *
    * Using AssemblyEntity::ID we ensure there won't be clashing between assemblies
    */
   class Assembly
      : public Entity {

   private:
      struct Connection {
         using Name = std::string;
         Name name;
         AssemblyEntity::ID src;
         AssemblyEntity::ID dst;
      };

      struct AssemblyNode {
         std::shared_ptr< AssemblyEntity > entity;

         // Connections by name and ids
         // Ensuring the connection makes sense (type, number, etc.) is left to views
         // TODO: does it make sense to have in/out connections? It should help to traverse the graph, right?
         std::unordered_map< Connection::Name, std::unordered_map< AssemblyEntity::ID, Connection > > connections;

         std::optional< Connection > getConnection( AssemblyEntity::ID otherID ) const
         {
            // Look for connections with entity
            return {};
         }

         std::optional< std::vector< Connection > > getConnections( Connection::Name name ) const
         {
            // Get connections for a given connection name
            return {};
         }

         void connect( Connection::Name, AssemblyEntity::ID otherID )
         {
            // TODO
         }

         void disconnect( Connection::Name, AssemblyEntity::ID otherID )
         {
            // TODO
         }
      };

   public:
      bool contains( std::shared_ptr< AssemblyEntity > const &entity ) const
      {
         return m_entities.contains( entity->getUniqueID() );
      }

      template< typename T >
      std::shared_ptr< T > emplace( void )
      {
         auto e = crimild::alloc< T >();
         e->setAssembly( std::static_pointer_cast< Assembly >( shared_from_this() ) );
         m_entities[ e->getUniqueID() ] = e;
         return e;
      }

      bool connect( AssemblyEntity::ID src, AssemblyEntity::ID dst, std::string_view pin )
      {
         return false;
      }

      template< typename T >
      std::shared_ptr< T > getConnection( AssemblyEntity::ID, Connection::Name name ) const
      {
         return nullptr;
      }

   private:
      std::unordered_map< AssemblyEntity::ID, std::shared_ptr< AssemblyEntity > > m_entities;
   };

   // Resources
   // An entity that can be consumed by other entities.
   class Resource : public AssemblyEntity { };
   class Material : public Resource { };
   class Primitive : public Resource { };

   template< typename T >
   class Value : public Resource {
   public:
      T get( void ) const { return m_value; }
      void set( T value ) { m_value = value; }

   private:
      T m_value;
   };

   class String : public Value< std::string > { };
   class Int : public Value< uint32_t > { };
   class Vector3 : public Value< crimild::Vector3f > { };

   // An assembly that is part of a hierarchy
   class Node : public AssemblyEntity {
   public:
      virtual ~Node( void ) = default;

      void setName( std::string name )
      {
         if ( auto value = getAssembly()->getConnection< String >( getUniqueID(), "name" ) ) {
            value->set( name );
         }
         m_name = name;
      }

      void setName( std::shared_ptr< String > const name )
      {
         // TODO
      }

      std::string getName( void ) const
      {
         if ( auto name = getAssembly()->getConnection< String >( getUniqueID(), "name" ) ) {
            return name->get();
         }
         return "";
      }

   private:
      // TODO: Should we keep this here or create a new entity by default?
      // If we create a new entity, it is possible that it ends up an orphan when
      // overriding it with a new one. But that can be solved by "trimming" the assembly
      // and removing orphans.
      // We could create a new entity lazily when attempting to get/set the value
      std::string m_name;
   };

   // A node that represents a transformation
   class Node3D : public Node { };
   class Node2D : public Node { };
   class Spatial3D : public Node { };
   class Spatial2D : public Node { };

   // A container of entities.
   class Group : public Entity { };
   class Bag : public Entity { };

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

TEST( Assembly, graph )
{
   auto A = crimild::alloc< crimild::next::Assembly >();

   auto n = A->emplace< crimild::next::Node >();
   auto s = A->emplace< crimild::next::String >();

   EXPECT_TRUE( A->contains( n ) );
   EXPECT_TRUE( A->contains( s ) );

   EXPECT_EQ( n->getName(), "" );

   n->setName( "foo" );
   EXPECT_EQ( n->getName(), "foo" );

   n->setName( s );
   EXPECT_EQ( n->getName(), "bar" );

   EXPECT_TRUE( true );
}
