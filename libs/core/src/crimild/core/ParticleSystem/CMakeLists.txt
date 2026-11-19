target_sources(
  crimild_core

  PRIVATE ParticleAttribArray.cpp
  PUBLIC ParticleAttribArray.hpp
  PRIVATE ParticleData.cpp
  PUBLIC ParticleData.hpp
  PRIVATE ParticleSystemComponent.cpp
  PUBLIC ParticleSystemComponent.hpp
)

add_subdirectory( Generators )
add_subdirectory( Renderers )
add_subdirectory( Updaters )
