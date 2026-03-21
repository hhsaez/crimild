#ifndef CRIMILD_CORE_NODES_VISITORS_VISITOR_
#define CRIMILD_CORE_NODES_VISITORS_VISITOR_

namespace crimild::nodes {

   class Node;

   class Spatial3D;

}

namespace crimild::nodes::visitors {

   class NodeVisitor {
   protected:
      NodeVisitor( void ) = default;

   public:
      virtual ~NodeVisitor( void ) = default;

      void traverse( Node & );

      void visitNode( nodes::Node & );

      void visitSpatial3D( nodes::Spatial3D & );
   };

   class ConstNodeVisitor {
   protected:
      ConstNodeVisitor( void ) = default;

   public:
      virtual ~ConstNodeVisitor( void ) = default;

      void traverse( const Node & );

      void visitNode( const Node & );

      void visitSpatial3D( const Spatial3D & );
   };

}

#endif
