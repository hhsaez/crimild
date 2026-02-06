#ifndef CRIMILD_OPENGL_RENDERING_RENDER_PASS_
#define CRIMILD_OPENGL_RENDERING_RENDER_PASS_

#include <memory>

namespace crimild::nodes {

   class Node;
   class Camera3D;

}

namespace crimild::universal {

   class RenderPass {
   public:
      void operator()(
         uint32_t width,
         uint32_t height,
         std::shared_ptr< crimild::nodes::Node > const &node,
         std::shared_ptr< crimild::nodes::Camera3D > const &camera
      ) const;

   private:
      void render(
         std::shared_ptr< crimild::nodes::Node > const &node,
         std::shared_ptr< crimild::nodes::Camera3D > const &camera
      ) const;
   };

}

#endif
