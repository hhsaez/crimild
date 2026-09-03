#ifndef CRIMILD_UNIVERSAL_SYSTEMS_DEBUG_
#define CRIMILD_UNIVERSAL_SYSTEMS_DEBUG_

#include <crimild/core/experimental/systems/System.hpp>
#include <crimild/math/ColorRGB.hpp>
#include <crimild/math/Normal3.hpp>
#include <crimild/math/Point3.hpp>

namespace crimild::universal::systems {

   class DebugSystem
      : public crimild::experimental::systems::System,
        public crimild::DynamicSingleton< DebugSystem > {

   public:
      static void addLine(
         const crimild::Point3 &from,
         const crimild::Point3 &to,
         const crimild::ColorRGB &color,
         crimild::real_t width = crimild::real_t( 1 ),
         crimild::real_t duration = crimild::real_t( 0 ),
         bool depthEnabled = true
      );

      static void addSphere(
         const crimild::Point3 &origin,
         crimild::real_t radius,
         const crimild::ColorRGB &color,
         crimild::real_t duration = crimild::real_t( 0 ),
         bool depthEnabled = true
      );

      static void addCircle(
         const crimild::Point3 &origin,
         crimild::real_t radius,
         const crimild::Normal3 &normal,
         const crimild::ColorRGB &color,
         crimild::real_t duration = crimild::real_t( 0 ),
         bool depthEnabled = true
      );

   public:
      DebugSystem( void ) noexcept;
      virtual ~DebugSystem( void ) noexcept = default;

      virtual State onUpdate( const crimild::experimental::systems::Context &ctx ) override;

      inline std::shared_ptr< crimild::experimental::Node > &getScene( void ) { return m_scene; }

   private:
      Clock m_clock;
      std::shared_ptr< crimild::experimental::Node > m_scene;
   };

}

#endif
