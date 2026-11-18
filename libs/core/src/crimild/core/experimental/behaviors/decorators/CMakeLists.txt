target_sources(
  crimild_core

  PRIVATE Decorator.cpp
  PUBLIC Decorator.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE Decorator.test.cpp
  )
endif ()
