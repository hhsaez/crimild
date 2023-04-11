crimild_trace()

add_library( crimild_opengl_player OBJECT )

target_sources(
    crimild_opengl_player

    PRIVATE
    Foundation/GLUtils.hpp
    Foundation/GLFWUtils.hpp
    
    Rendering/FrameGraph/GLFrameGraphNode.hpp
    Rendering/FrameGraph/GLRenderScene.cpp
    Rendering/FrameGraph/GLRenderScene.hpp

    Rendering/GLContext.hpp
    Rendering/GLSceneRenderState.hpp

    Visitors/GLFetchSceneRenderState.cpp
    Visitors/GLFetchSceneRenderState.hpp

    Main.cpp

    PRIVATE ${PROJECT_SOURCE_DIR}/third-party/glad/src/glad.c
)

target_include_directories(
    crimild_opengl_player

    PRIVATE .    
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/glad/include
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/glfw/include
)

target_link_libraries(
    crimild_opengl_player
    PRIVATE Crimild::Core
    PRIVATE glfw
)

add_library( Crimild::OpenGL::Player ALIAS crimild_opengl_player )

