#include <Crimild.hpp>
#include <Crimild_UniversalPlayer.hpp>

namespace crimild::experimental {

   class Bounding3D : public Spatial3D {
   public:
      virtual ~Bounding3D( void ) = default;

      bool intersects( const Ray3 &ray ) const
      {
         auto localRay = inverse( getWorld() )( ray );
         float t0, t1;
         auto ret = intersect( localRay, Sphere {}, t0, t1 );
         return ret;
      }
   };

   class Selectable : public Node {
   public:
      Signal< std::shared_ptr< Selectable > > selected;
      Signal< std::shared_ptr< Selectable > > deselected;

   public:
      virtual ~Selectable( void ) = default;

      void select( void )
      {
         selected( retain( this ) );
      }

      void deselect( void )
      {
         deselected( retain( this ) );
      }
   };

   class Select : public NodeReducerVisitor< std::shared_ptr< Selectable > > {
   public:
      Select( const crimild::Ray3 &ray ) : m_ray( ray ) { }
      virtual ~Select( void ) = default;

      virtual void visitNode( Node &node ) override
      {
         NodeVisitor::visitNode( node );

         if ( getResult() != nullptr ) {
            return;
         }

         if ( auto bounding = dynamic_cast< Bounding3D * >( &node ) ) {
            if ( bounding->intersects( m_ray ) ) {
               auto node = bounding->getParent();
               while ( node != nullptr ) {
                  if ( auto result = std::dynamic_pointer_cast< Selectable >( node ) ) {
                     setResult( result );
                     break;
                  }
                  node = node->getParent();
               }
            }
         }
      }

   private:
      crimild::Ray3 m_ray;
      std::shared_ptr< Selectable > m_result;
   };

}

using namespace crimild;
using namespace crimild::universal;
using namespace crimild::experimental;

std::shared_ptr< experimental::Node > createTriangle( float x, float y )
{
   static auto sharedPrimitive = [] {
      auto primitive = std::make_shared< Primitive >( Primitive::Type::TRIANGLES );
      primitive->setVertexData(
         {
            [] {
               return std::make_shared< VertexBuffer >(
                  VertexP3TC2::getLayout(),
                  Array< VertexP3TC2 > {
                     {
                        .position = Vector3f { -0.5f, -0.5f, 0.0f },
                        .texCoord = Vector2f { 0, 0 },
                     },
                     {
                        .position = Vector3f { 0.5f, -0.5f, 0.0f },
                        .texCoord = Vector2f { 0, 1 },
                     },
                     {
                        .position = Vector3f { 0.0f, 0.5f, 0.0f },
                        .texCoord = Vector2f { 1, 1 },
                     },
                  }
               );
            }(),
         }
      );
      primitive->setIndices(
         std::make_shared< IndexBuffer >(
            Format::INDEX_32_UINT,
            Array< UInt32 > { 0, 1, 2 }
         )
      );
      return primitive;
   }();

   auto group = std::make_shared< Spatial3D >();
   auto geometry = std::make_shared< Geometry3D >();
   geometry->setPrimitive( sharedPrimitive );
   geometry->setMaterial(
      [] {
         auto material = std::make_shared< UnlitMaterial >();
         material->setColor( ColorRGBA { 1.0f, 0.0f, 0.0f, 1.0f } );
         return material;
      }()
   );
   group->attach( geometry );

   auto selectable = std::make_shared< Selectable >();
   selectable->attach< Bounding3D >();
   selectable->selected.bind(
      []( std::shared_ptr< Selectable > selectable ) {
         auto geometry = selectable->getParent< Spatial3D >()->getChildAt< Geometry3D >( 0 );
         if ( auto material = geometry->getMaterial< UnlitMaterial >() ) {
            material->setColor( ColorRGBA { 0.0f, 1.0f, 0.0f, 1.0f } );
         }
      }
   );
   selectable->deselected.bind(
      []( std::shared_ptr< Selectable > selectable ) {
         auto geometry = selectable->getParent< Spatial3D >()->getChildAt< Geometry3D >( 0 );
         if ( auto material = geometry->getMaterial< UnlitMaterial >() ) {
            material->setColor( ColorRGBA { 1.0f, 0.5f, 0.0f, 1.0f } );
         }
      }
   );
   group->attach( selectable );

   group->setLocal( translation( x, y, 0 ) );

   return group;
}

auto main( int argc, char *argv[] ) -> int
{
   crimild::init();
   CRIMILD_LOG_INFO( crimild::Version().getFullDescription() );

   crimild::Log::setOutputHandlers(
      {
         std::make_shared< ConsoleOutputHandler >( Log::LOG_LEVEL_ALL, true ),
      }
   );

   GLFWSystem glfwSystem;
   if ( !glfwSystem.isInitialized() ) {
      return -1;
   }

   GLFWOpenGLWindow window( 1024, 768, "Bounding3D" );
   if ( !window.isValid() ) {
      return -1;
   }

   auto scene = std::make_shared< crimild::experimental::Node >();
   for ( float x = -3; x <= 3; x += 1.5f ) {
      for ( float y = -3; y <= 3; y += 1.5f ) {
         scene->attach( createTriangle( x, y ) );
      }
   }

   auto camera = std::make_shared< Camera3D >();
   camera->setLocal(
      lookAt(
         Point3f { 0, 0, 10 },
         Point3f { 0, 0, 0 },
         Vector3f::Constants::UP
      )
   );

   window.onMouseButtonReleased.bind(
      [ &window,
        maybeScene = std::weak_ptr< experimental::Node >( scene ),
        maybeCamera = std::weak_ptr< Camera3D >( camera ) ]( int button, double x, double y ) {
         const auto width = window.getWidth();
         const auto height = window.getHeight();
         static std::weak_ptr< Selectable > prev;
         if ( button == CRIMILD_INPUT_MOUSE_BUTTON_LEFT ) {
            if ( auto prevSelected = prev.lock() ) {
               prevSelected->deselect();
            }
            if ( auto camera = maybeCamera.lock() ) {
               auto ray = camera->getPickRay( { float( x ) / width, float( y ) / height } );
               if ( auto scene = maybeScene.lock() ) {
                  if ( auto selectable = scene->perform< Select >( ray ) ) {
                     selectable->select();
                     prev = selectable;
                  }
               }
            }
         }
      }
   );

   crimild::universal::RenderPass render;

   while ( window.isOpen() ) {
      const auto [ w, h ] = window.getRenderSize();
      camera->setProjectionMatrix( perspective( 60.0f, ( float ) w / ( float ) h, 0.1f, 1000.0f ) );
      render( w, h, scene, camera );
      window.swapBuffers();
      glfwSystem.update();
   }

   return 0;
}
