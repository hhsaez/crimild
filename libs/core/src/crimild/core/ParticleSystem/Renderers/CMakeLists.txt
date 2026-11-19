target_sources(
  crimild_core

  PRIVATE AnimatedSpriteParticleRenderer.cpp
  PUBLIC AnimatedSpriteParticleRenderer.hpp
  PRIVATE InstancedParticleRenderer.cpp
  PUBLIC InstancedParticleRenderer.hpp
  PRIVATE NodeParticleRenderer.cpp
  PUBLIC NodeParticleRenderer.hpp
  PRIVATE OrientedQuadParticleRenderer.cpp
  PUBLIC OrientedQuadParticleRenderer.hpp
  PRIVATE PointSpriteParticleRenderer.cpp
  PUBLIC PointSpriteParticleRenderer.hpp
)
