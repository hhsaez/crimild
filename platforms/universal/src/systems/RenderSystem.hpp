#ifndef CRIMILD_UNIVERSAL_SYSTEMS_RENDER_
#define CRIMILD_UNIVERSAL_SYSTEMS_RENDER_

#include <crimild/core/experimental/systems/System.hpp>
#include <crimild/math/ColorRGBA.hpp>
#include <crimild/math/Rect.hpp>
#include <memory>
#include <set>

namespace crimild {
   namespace experimental {
      class Node;
      class Camera3D;
   }

   namespace universal {
      class Bindable;
   }
}

namespace crimild::universal::systems {

   class RenderSystem : public crimild::experimental::systems::System {
   private:
      using State = crimild::experimental::systems::System::State;
      using Context = crimild::experimental::systems::Context;

   public:
      virtual ~RenderSystem( void ) noexcept;

      virtual State onUpdate( const Context &ctx ) override;

      void bind( std::shared_ptr< Bindable > const &resource ) const
      {
         m_resources.insert( std::weak_ptr( resource ) );
      }

   private:
      void render(
         Rect viewport,
         std::shared_ptr< crimild::experimental::Node > const &node,
         std::shared_ptr< crimild::experimental::Camera3D > const &camera,
         ColorRGBA clearColor = ColorRGBA { 0.0f, 0.0f, 0.0f, 1.0f }
      ) const;

      /**
       * @brief Renders a scene without clearning
       */
      void render(
         std::shared_ptr< crimild::experimental::Node > const &node,
         std::shared_ptr< crimild::experimental::Camera3D > const &camera
      ) const;

   private:
      mutable std::set< std::weak_ptr< Bindable >, std::owner_less<> > m_resources;
   };

}

#endif
