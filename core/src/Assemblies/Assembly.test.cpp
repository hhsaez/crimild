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

      std::vector< GraphNodeId > getNeighbors( void ) const
      {
         std::vector< GraphNodeId > ret;
         for ( const auto &e : edges ) {
            ret.push_back( e.dst );
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
