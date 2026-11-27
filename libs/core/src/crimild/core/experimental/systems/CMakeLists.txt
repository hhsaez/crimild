target_sources(
  crimild_core

  PRIVATE Runner.cpp
  PUBLIC Runner.hpp
  PRIVATE System.cpp
  PUBLIC System.hpp
  PRIVATE TickSystem.cpp
  PUBLIC TickSystem.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE TickSystem.test.cpp
  )
endif ()
