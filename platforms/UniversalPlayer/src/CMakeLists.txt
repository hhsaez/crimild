crimild_trace()

add_library( crimild_universal_player OBJECT )

target_sources(
   crimild_universal_player

   PUBLIC Crimild_UniversalPlayer.hpp
   PRIVATE Crimild_UniversalPlayer.cpp

   PUBLIC Foundation/GLFWSystem.hpp
   PRIVATE Foundation/GLFWSystem.cpp
   PUBLIC Foundation/GLFWOpenGLWindow.hpp
   PRIVATE Foundation/GLFWOpenGLWindow.cpp
   PRIVATE Foundation/OpenGLUtils.hpp  
   PRIVATE Foundation/OpenGLUtils.cpp

   PRIVATE Rendering/OpenGLBindable.hpp
   PRIVATE Rendering/OpenGLIndexBufferBindable.cpp
   PRIVATE Rendering/OpenGLIndexBufferBindable.hpp
   PRIVATE Rendering/OpenGLMaterialBindable.cpp
   PRIVATE Rendering/OpenGLMaterialBindable.hpp
   PRIVATE Rendering/OpenGLPrimitiveBindable.cpp
   PRIVATE Rendering/OpenGLPrimitiveBindable.hpp
   PRIVATE Rendering/OpenGLSceneRenderPass.cpp
   PUBLIC Rendering/OpenGLSceneRenderPass.hpp
   PRIVATE Rendering/OpenGLShader.cpp
   PRIVATE Rendering/OpenGLShader.hpp
   PRIVATE Rendering/OpenGLShaderProgram.cpp
   PRIVATE Rendering/OpenGLShaderProgram.hpp
   PRIVATE Rendering/OpenGLTextureBindable.cpp
   PRIVATE Rendering/OpenGLTextureBindable.hpp
   PRIVATE Rendering/OpenGLVertexBufferBindable.cpp
   PRIVATE Rendering/OpenGLVertexBufferBindable.hpp
)

target_include_directories(
   crimild_universal_player

   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
   PRIVATE ${Crimild_SOURCE_DIR}/third-party/glfw/include
   PRIVATE ${Crimild_SOURCE_DIR}/third-party/glad/include
)

target_link_libraries(
   crimild_universal_player

   PRIVATE Crimild::Foundation
   PRIVATE Crimild::Mathematics
   PRIVATE Crimild::Coding
   PRIVATE Crimild::Core
   PRIVATE glfw
)

add_library( Crimild::UniversalPlayer ALIAS crimild_universal_player )
