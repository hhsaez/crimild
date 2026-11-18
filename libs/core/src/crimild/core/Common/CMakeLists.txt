target_sources(
  crimild_core

  PRIVATE Profiler.cpp
  PUBLIC Profiler.hpp
  PUBLIC Signal.hpp
  PRIVATE Variant.cpp
  PUBLIC Variant.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE Signal.test.cpp
    PRIVATE Variant.test.cpp
  )
endif ()
