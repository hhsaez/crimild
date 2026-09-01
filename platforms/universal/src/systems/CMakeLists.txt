crimild_trace()

target_sources(
  crimild_universal

  PRIVATE GLFWSystem.cpp
  PRIVATE GLFWWindow.cpp
  PRIVATE RenderSystem.hpp
  PRIVATE RenderSystem.cpp
  PRIVATE UniversalSceneRunner.cpp
)
