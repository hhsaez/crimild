#include "Assembly.hpp"

#include "Entity/Entity.hpp"

#include <gtest/gtest.h>

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
   };

   template< typename T >
   struct Graph {
      std::optional< GraphEdge > getEdge( GraphNodeId src, GraphNodeId dst ) const
      {
         return {};
      }

   private:
      std::vector< GraphNode< T > > m_nodes;
   };

}

TEST( Graph, test )
{
   EXPECT_TRUE( true );
}
