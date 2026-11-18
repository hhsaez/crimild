target_sources(
  crimild_core

  PRIVATE InvalidateHierarchy.cpp
  PUBLIC InvalidateHierarchy.hpp
  PRIVATE InvalidateWorldState.cpp
  PUBLIC InvalidateWorldState.hpp
  PRIVATE NodeConstVisitor.cpp
  PUBLIC NodeConstVisitor.hpp
  PRIVATE NodeVisitor.cpp
  PUBLIC NodeVisitor.hpp
  PUBLIC Select.hpp
  PUBLIC Tick.hpp
)
