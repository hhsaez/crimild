#ifndef CRIMILD_CORE_NODES_VISITORS_CONST_VISITOR_
#define CRIMILD_CORE_NODES_VISITORS_CONST_VISITOR_

namespace crimild::experimental {

   class Node;
   class Group;

   class Spatial3D;
   class Group3D;

   class NodeConstVisitor {
   protected:
      NodeConstVisitor( void ) = default;

   public:
      virtual ~NodeConstVisitor( void ) = default;

      virtual void traverse( const Node & );

      virtual void visitNode( const Node & );
      virtual void visitGroup( const Group & );

      virtual void visitSpatial3D( const Spatial3D & );
      virtual void visitGroup3D( const Group3D & );
   };

}

#endif
