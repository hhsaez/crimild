#ifndef CRIMILD_OPENGL_RENDERING_RENDER_PASS_SCENE_
#define CRIMILD_OPENGL_RENDERING_RENDER_PASS_SCENE_

#include <memory>

namespace crimild {

   class Node;
   class Camera;

}

namespace crimild::universal {

   class SceneRenderPass {
   public:
      void operator()(
         uint32_t width,
         uint32_t height,
         std::shared_ptr< crimild::Node > const &scene,
         std::shared_ptr< crimild::Camera > const &camera
      );
   };

}

#endif
