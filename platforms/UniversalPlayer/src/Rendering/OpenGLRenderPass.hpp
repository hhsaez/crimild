#ifndef CRIMILD_OPENGL_RENDERING_RENDER_PASS_
#define CRIMILD_OPENGL_RENDERING_RENDER_PASS_

#include <crimild/math/ColorRGBA.hpp>
#include <crimild/math/Rect.hpp>
#include <memory>

namespace crimild {

   class Material;

   namespace experimental {

      class Node;
      class Camera3D;

   }

   namespace opengl {

      class MaterialBindable;

   }

}

namespace crimild::universal {

   class RenderPass {
   public:
      void operator()(
         Rect viewport,
         std::shared_ptr< crimild::experimental::Node > const &node,
         std::shared_ptr< crimild::experimental::Camera3D > const &camera,
         ColorRGBA clearColor = ColorRGBA { 0.0f, 0.0f, 0.0f, 1.0f }
      ) const;

      /**
       * @brief Renders a scene without clearning
       */
      void operator()(
         std::shared_ptr< crimild::experimental::Node > const &node,
         std::shared_ptr< crimild::experimental::Camera3D > const &camera
      ) const;
   };

}

#endif
