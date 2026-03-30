#ifndef CRIMILD_CORE_NODES_VISITORS_VISITOR_
#define CRIMILD_CORE_NODES_VISITORS_VISITOR_

namespace crimild::experimental {

   class Node;

   class Spatial3D;
   class Group3D;

   class NodeVisitor {
   protected:
      NodeVisitor( void ) = default;

   public:
      virtual ~NodeVisitor( void ) = default;

      virtual void traverse( Node & );

      virtual void visitNode( Node & );

      virtual void visitSpatial3D( Spatial3D & );
      virtual void visitGroup3D( Group3D & );
   };

}

#endif
