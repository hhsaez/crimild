target_sources(
  crimild_core

  PRIVATE Behavior.cpp
  PRIVATE Behavior.hpp
  PUBLIC README.md
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE Behavior.test.cpp
  )
endif ()

add_subdirectory( composites )
add_subdirectory( decorators )
