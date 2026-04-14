#ifndef CRIMILD_CORE_NODES_VISITORS_CONST_VISITOR_
#define CRIMILD_CORE_NODES_VISITORS_CONST_VISITOR_

namespace crimild::experimental {

   class Node;

   class Spatial3D;
   class Geometry3D;

   class NodeConstVisitor {
   protected:
      NodeConstVisitor( void ) = default;

   public:
      virtual ~NodeConstVisitor( void ) = default;

      virtual void traverse( const Node & );

      virtual void visitNode( const Node & );

      virtual void visitSpatial3D( const Spatial3D & );
      virtual void visitGeometry3D( const Geometry3D & );
   };

   class NodeVoidConstVisitor : public NodeConstVisitor {
   public:
      using ResultType = void;

      virtual ~NodeVoidConstVisitor( void ) = default;
   };

   template< typename ReducerResultType >
   class NodeReducerConstVisitor : public NodeConstVisitor {
   public:
      using ResultType = ReducerResultType;
      ResultType getResult( void ) const { return m_result; }

      virtual ~NodeReducerConstVisitor( void ) = default;

   protected:
      void setResult( const ResultType &result ) { m_result = result; }

   private:
      ResultType m_result;
   };

}

#endif
