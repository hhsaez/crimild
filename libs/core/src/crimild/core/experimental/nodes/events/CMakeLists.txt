target_sources(
  crimild_core

  PRIVATE Event.cpp
  PUBLIC Event.hpp
  PRIVATE OnTick.cpp
  PUBLIC OnTick.hpp
  PUBLIC README.md
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE Event.test.cpp
    PRIVATE OnTick.test.cpp
  )
endif ()
