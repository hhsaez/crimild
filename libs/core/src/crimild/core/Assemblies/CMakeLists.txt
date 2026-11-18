target_sources(
  crimild_core

  PRIVATE Assembly.cpp
  PUBLIC Assembly.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE Assembly.test.cpp
    PRIVATE AssemblyDAG.test.cpp
  )
endif ()
