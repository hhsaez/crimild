target_sources(
  crimild_core

  PUBLIC AudioSystem.hpp
  PRIVATE CaptureSystem.cpp
  PUBLIC CaptureSystem.hpp
  PRIVATE ConsoleSystem.cpp
  PUBLIC ConsoleSystem.hpp
  PRIVATE DebugSystem.cpp
  PUBLIC DebugSystem.hpp
  PRIVATE RenderSystem.cpp
  PUBLIC RenderSystem.hpp
  PRIVATE StreamingSystem.cpp
  PUBLIC StreamingSystem.hpp
  PRIVATE System.cpp
  PUBLIC System.hpp
  PRIVATE UISystem.cpp
  PUBLIC UISystem.hpp
  PRIVATE UpdateSystem.cpp
  PUBLIC UpdateSystem.hpp
)
