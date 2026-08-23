add_subdirectory( composites )
add_subdirectory( decorators )

target_sources(
  crimild_core

  PRIVATE Behavior.hpp
)
