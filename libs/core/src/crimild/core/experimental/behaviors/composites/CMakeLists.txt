target_sources(
  crimild_core

  PRIVATE ActiveSelector.cpp
  PUBLIC ActiveSelector.hpp
  PRIVATE ActiveSequence.cpp
  PUBLIC ActiveSequence.hpp
  PRIVATE Composite.cpp
  PUBLIC Composite.hpp
  PRIVATE Parallel.cpp
  PUBLIC Parallel.hpp
  PRIVATE Selector.cpp
  PUBLIC Selector.hpp
  PRIVATE Sequence.cpp
  PUBLIC Sequence.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE ActiveSelector.test.cpp
    PRIVATE ActiveSequence.test.cpp
    PRIVATE Composite.test.cpp
    PRIVATE Parallel.test.cpp
    PRIVATE Selector.test.cpp
    PRIVATE Sequence.test.cpp
  )
endif ()
