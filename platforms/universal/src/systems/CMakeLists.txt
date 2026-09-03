crimild_trace()

target_sources(
  crimild_universal

  PRIVATE GLFWInputSystem.hpp
  PRIVATE GLFWInputSystem.cpp
  PRIVATE GLFWSystem.cpp
  PRIVATE GLFWWindow.cpp
  PRIVATE RenderSystem.hpp
  PRIVATE RenderSystem.cpp
  PRIVATE SimulationSystem.hpp
  PRIVATE SimulationSystem.cpp
  
  PRIVATE UniversalSceneRunner.cpp
)
