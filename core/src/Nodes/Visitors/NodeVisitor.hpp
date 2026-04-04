#ifndef CRIMILD_CORE_NODES_VISITORS_VISITOR_
#define CRIMILD_CORE_NODES_VISITORS_VISITOR_

namespace crimild::experimental {

   class Node;
   class Group;

   class Spatial3D;
   class Geometry3D;
   class Group3D;

   class NodeVisitor {
   protected:
      NodeVisitor( void ) = default;

   public:
      virtual ~NodeVisitor( void ) = default;

      virtual void traverse( Node & );

      virtual void visitNode( Node & );
      virtual void visitGroup( Group & );

      virtual void visitSpatial3D( Spatial3D & );
      virtual void visitGeometry3D( Geometry3D & );
      virtual void visitGroup3D( Group3D & );
   };

   class NodeVoidVisitor : public NodeVisitor {
   public:
      using ResultType = void;

      virtual ~NodeVoidVisitor( void ) = default;
   };

   template< typename ReducerResultType >
   class NodeReducerVisitor : public NodeVisitor {
   public:
      using ResultType = ReducerResultType;
      ResultType getResult( void ) const { return m_result; }

      virtual ~NodeReducerVisitor( void ) = default;

   protected:
      void setResult( const ResultType &result ) { m_result = result; }

   private:
      ResultType m_result;
   };

}

#endif
