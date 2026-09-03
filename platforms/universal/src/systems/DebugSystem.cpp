#include <crimild/core/Primitives/Primitive.hpp>
#include <crimild/core/Primitives/SpherePrimitive.hpp>
#include <crimild/core/Rendering/Materials/UnlitMaterial.hpp>
#include <crimild/core/experimental/nodes/3d/Geometry3D.hpp>
#include <crimild/core/experimental/nodes/events/OnTick.hpp>
#include <crimild/core/experimental/nodes/visitors/Tick.hpp>
#include <crimild/math/Vector3.hpp>
#include <crimild/math/isNaN.hpp>
#include <crimild/math/numbers.hpp>
#include <crimild/math/orthonormalization.hpp>
#include <crimild/math/scale.hpp>
#include <crimild/math/translation.hpp>
#include <crimild/universal/systems/DebugSystem.hpp>

namespace crimild::universal::systems {

   class DebugGeometry3D : public crimild::experimental::Geometry3D {
   public:
      DebugGeometry3D( void ) noexcept
      {
         attach< crimild::experimental::OnTick >()->triggered.bind( this, &DebugGeometry3D::onTick );
      }

      virtual ~DebugGeometry3D( void ) noexcept = default;

   private:
      virtual void onTick( const crimild::Clock &clock )
      {
         if ( !isEnabled() ) {
            return;
         }

         m_decayTime -= clock.getDeltaTime();
         if ( m_decayTime <= 0 ) {
            setEnabled( false );
         }
      }

   protected:
      virtual void reset( float decayTime )
      {
         m_decayTime = decayTime;
         setEnabled( true );
      }

   private:
      float m_decayTime = 0.0f;
   };

   class DebugLine3D : public DebugGeometry3D {
   public:
      DebugLine3D( void ) noexcept
      {
         using namespace crimild;

         // Lines are rendered individually, using a single primitive for all lines.
         // This might lead to some performance issues when rendering many lines, but I'll deal with that later.
         static auto primitive = [] {
            auto primitive = std::make_shared< Primitive >( Primitive::Type::LINES );
            const auto layout = VertexLayout::P3_N3_TC2;

            auto vertices = std::make_shared< VertexBuffer >( layout, 2 );
            auto positions = vertices->get( VertexAttribute::Name::POSITION );
            positions->set( 0, Point3f { 0, 0, 0 } );
            positions->set( 1, Point3f { 0, 0, -1 } );
            primitive->setVertexData( { vertices } );

            auto indices = std::make_shared< IndexBuffer >( Format::INDEX_32_UINT, Array< UInt32 > { 0, 1 } );
            primitive->setIndices( indices );
            return primitive;
         }();

         setPrimitive( primitive );
         setMaterial(
            [] {
               auto material = std::make_shared< crimild::UnlitMaterial >();
               material->setColor( crimild::ColorRGBA { 1, 0, 1, 1 } );
               return material;
            }()
         );
      }

      virtual ~DebugLine3D( void ) noexcept = default;

      void reset(
         const crimild::Point3 &from,
         const crimild::Point3 &to,
         const crimild::ColorRGB &color,
         crimild::real_t width,
         crimild::real_t duration,
         bool depthEnabled
      )
      {
         using namespace crimild;

         const auto up = [ & ] {
            auto ret = cross( Vector3f( to ), Vector3f( from ) );
            const auto retLengthSquared = length2( ret );
            if ( !isNaN( ret ) && !isNaN( retLengthSquared ) && retLengthSquared > 0 ) {
               return ret;
            } else {
               Vector3f right, up;
               orthonormalBasis( normalize( Vector3f( to ) ), right, up );
               if ( isNaN( up ) || isZero( length2( up ) ) ) {
                  return Vector3f { 0, 1, 0 };
               }
               return up;
            }
         }();

         DebugGeometry3D::reset( duration );
      }
   };

   class DebugSphere3D : public DebugGeometry3D {
   public:
      DebugSphere3D( void ) noexcept
      {
         auto primitive = std::make_shared< crimild::SpherePrimitive >(
            crimild::SpherePrimitive::Params {
               .type = crimild::Primitive::Type::LINES,
               .radius = 1.0f,
               .layout = crimild::VertexP3TC2::getLayout(),
               .divisions = crimild::Vector2i { 10, 10 },
            }
         );
         setPrimitive( primitive );
         setMaterial(
            [] {
               auto material = std::make_shared< crimild::UnlitMaterial >();
               material->setColor( crimild::ColorRGBA { 1, 0, 1, 1 } );
               return material;
            }()
         );
      }

      virtual ~DebugSphere3D( void ) noexcept = default;

      void reset(
         const crimild::Point3 &origin,
         crimild::real_t radius,
         const crimild::ColorRGB &color,
         crimild::real_t duration,
         bool depthEnabled
      )
      {
         using namespace crimild;

         setLocal( translation( origin )( scale( radius ) ) );
         getMaterial< UnlitMaterial >()->setColor( rgba( color ) );

         DebugGeometry3D::reset( duration );
      }
   };

   class DebugCircle3D : public DebugGeometry3D {
   public:
      DebugCircle3D( void ) noexcept
      {
         static auto primitive = [] {
            auto primitive = std::make_shared< crimild::Primitive >( crimild::Primitive::Type::LINE_STRIP );

            const size_t segments = 10;
            crimild::Array< crimild::VertexP3TC2 > vertices( segments + 1 );
            float phi = 0.0f;
            float inc = crimild::numbers::TWO_PI / segments;
            for ( size_t i = 0; i <= segments; i++ ) {
               vertices[ i ] = {
                  .position = crimild::Vector3f(
                     std::cos( phi ),
                     std::sin( phi ),
                     0.0f
                  ),
               };
               phi += inc;
            }

            primitive->setVertexData(
               {
                  std::make_shared< crimild::VertexBuffer >(
                     crimild::VertexP3TC2::getLayout(),
                     vertices
                  ),
               }
            );

            crimild::Array< uint32_t > indices( vertices.size() );
            for ( uint32_t i = 0; i < indices.size(); i++ ) {
               indices[ i ] = i;
            }
            primitive->setIndices(
               std::make_shared< crimild::IndexBuffer >(
                  crimild::Format::INDEX_32_UINT,
                  indices
               )
            );

            return primitive;
         }();
         setPrimitive( primitive );
         setMaterial(
            [] {
               auto material = std::make_shared< crimild::UnlitMaterial >();
               material->setColor( crimild::ColorRGBA { 1, 0, 1, 1 } );
               return material;
            }()
         );
      }

      virtual ~DebugCircle3D( void ) noexcept = default;

      void reset(
         const crimild::Point3 &origin,
         crimild::real_t radius,
         const crimild::Normal3 &normal,
         const crimild::ColorRGB &color,
         crimild::real_t duration,
         bool depthEnabled
      )
      {
         using namespace crimild;

         setLocal( translation( origin )( scale( radius ) ) );
         getMaterial< UnlitMaterial >()->setColor( rgba( color ) );

         DebugGeometry3D::reset( duration );
      }
   };

   struct DebugPrimitives {
      enum {
         LINES = 0,
         SPHERES = 1,
         CIRCLES = 2,

         MAX_COUNT,
      };
   };

}

using namespace crimild::universal::systems;

void DebugSystem::addLine(
   const crimild::Point3 &from,
   const crimild::Point3 &to,
   const crimild::ColorRGB &color,
   crimild::real_t width,
   crimild::real_t duration,
   bool depthEnabled
)
{
   using namespace crimild;

   if ( isEqual( to, from ) ) {
      return;
   }

   if ( auto system = DebugSystem::getInstance() ) {
      auto lines = system->getScene()->getChildAt< experimental::Node >( DebugPrimitives::LINES );
      std::shared_ptr< DebugLine3D > line;
      for ( auto &c : lines->getChildren() ) {
         if ( !c->isEnabled() ) {
            line = std::dynamic_pointer_cast< DebugLine3D >( c );
         }
      }
      if ( line == nullptr ) {
         line = lines->attach< DebugLine3D >();
      }

      line->reset( from, to, color, width, duration, depthEnabled );
   }
}

void DebugSystem::addSphere(
   const crimild::Point3 &origin,
   crimild::real_t radius,
   const crimild::ColorRGB &color,
   crimild::real_t duration,
   bool depthEnabled
)
{
   using namespace crimild;

   if ( isZero( radius ) ) {
      return;
   }

   if ( auto system = DebugSystem::getInstance() ) {
      auto spheres = system->getScene()->getChildAt< experimental::Node >( DebugPrimitives::SPHERES );
      std::shared_ptr< DebugSphere3D > sphere;
      for ( auto &c : spheres->getChildren() ) {
         if ( !c->isEnabled() ) {
            sphere = std::dynamic_pointer_cast< DebugSphere3D >( c );
         }
      }
      if ( sphere == nullptr ) {
         sphere = spheres->attach< DebugSphere3D >();
      }

      sphere->reset( origin, radius, color, duration, depthEnabled );
   }
}

void DebugSystem::addCircle(
   const crimild::Point3 &origin,
   crimild::real_t radius,
   const crimild::Normal3 &normal,
   const crimild::ColorRGB &color,
   crimild::real_t duration,
   bool depthEnabled
)
{
   using namespace crimild;

   if ( isZero( radius ) ) {
      return;
   }

   if ( auto system = DebugSystem::getInstance() ) {
      auto circles = system->getScene()->getChildAt< experimental::Node >( DebugPrimitives::CIRCLES );
      std::shared_ptr< DebugCircle3D > circle;
      for ( auto &c : circles->getChildren() ) {
         if ( !c->isEnabled() ) {
            circle = std::dynamic_pointer_cast< DebugCircle3D >( c );
         }
      }
      if ( circle == nullptr ) {
         circle = circles->attach< DebugCircle3D >();
      }

      circle->reset( origin, radius, normal, color, duration, depthEnabled );
   }
}

DebugSystem::DebugSystem( void ) noexcept
{
   m_scene = std::make_shared< crimild::experimental::Node >();
   for ( size_t i = 0; i < DebugPrimitives::MAX_COUNT; i++ ) {
      m_scene->attach< crimild::experimental::Node >();
   }

   // Debug system is disabled by default
   setEnabled( false );
}

DebugSystem::State DebugSystem::onUpdate( const crimild::experimental::systems::Context &ctx )
{
   m_clock.tick();

   // Update debug system regardless of its enabled state
   // That way, existing debug shapes get remvoed behind the scenes when
   // they expire.
   if ( auto debugScene = getScene() ) {
      // Do debug tick before scene tick to release expired elements before adding new ones
      // Don't delete elements. Disable them instead so we can reuse them later.
      debugScene->perform< crimild::experimental::Tick >( m_clock );
   }

   return System::onUpdate( ctx );
}
