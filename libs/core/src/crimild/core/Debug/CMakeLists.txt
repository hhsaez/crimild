target_sources(
  crimild_core

  PRIVATE DebugDrawManager.cpp
  PUBLIC DebugDrawManager.hpp
  PRIVATE DebugRenderComponent.cpp
  PUBLIC DebugRenderComponent.hpp
  PRIVATE DebugRenderHelper.cpp
  PUBLIC DebugRenderHelper.hpp
  PRIVATE SceneDebugDump.cpp
  PUBLIC SceneDebugDump.hpp
)
