add_subdirectory( 3d )
add_subdirectory( events )
add_subdirectory( visitors )

target_sources(
  crimild_core

  PRIVATE Node.hpp
)
