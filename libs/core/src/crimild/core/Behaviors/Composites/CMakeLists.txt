target_sources(
  crimild_core

  PRIVATE Composite.cpp
  PUBLIC Composite.hpp
  PRIVATE Parallel.cpp
  PUBLIC Parallel.hpp
  PRIVATE Selector.cpp
  PUBLIC Selector.hpp
  PRIVATE Sequence.cpp
  PUBLIC Sequence.hpp
)
