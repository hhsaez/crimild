target_sources(
  crimild_core

  PRIVATE Node.cpp
  PRIVATE Node.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE Node.test.cpp
  )
endif ()

add_subdirectory( 3d )
add_subdirectory( events )
add_subdirectory( visitors )
