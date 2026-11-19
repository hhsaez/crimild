target_sources(
  crimild_core

  PRIVATE init.cpp
  PUBLIC init.hpp
)

add_subdirectory( Animation )
add_subdirectory( Assemblies )
add_subdirectory( Audio )
add_subdirectory( Behaviors )
add_subdirectory( Boundings )
add_subdirectory( Common )
add_subdirectory( Components )
add_subdirectory( Concurrency )
add_subdirectory( Debug )
add_subdirectory( Entity )
add_subdirectory( Exceptions )
add_subdirectory( experimental )
add_subdirectory( Loaders )
add_subdirectory( Messaging )
add_subdirectory( Navigation )
add_subdirectory( ParticleSystem )
add_subdirectory( Primitives )
add_subdirectory( Rendering )
add_subdirectory( SceneGraph )
add_subdirectory( Simulation )
add_subdirectory( UI )
add_subdirectory( Visitors )
