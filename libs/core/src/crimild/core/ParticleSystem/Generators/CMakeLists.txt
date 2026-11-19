target_sources(
  crimild_core

  PRIVATE AccelerationParticleGenerator.cpp
  PUBLIC AccelerationParticleGenerator.hpp
  PRIVATE BoxPositionParticleGenerator.cpp
  PUBLIC BoxPositionParticleGenerator.hpp
  PRIVATE CirclePositionParticleGenerator.cpp
  PUBLIC CirclePositionParticleGenerator.hpp
  PRIVATE ColorParticleGenerator.cpp
  PUBLIC ColorParticleGenerator.hpp
  PUBLIC DefaultValueParticleGenerator.hpp
  PRIVATE EulerAnglesParticleGenerator.cpp
  PUBLIC EulerAnglesParticleGenerator.hpp
  PRIVATE GridPositionParticleGenerator.cpp
  PUBLIC GridPositionParticleGenerator.hpp
  PRIVATE NodePositionParticleGenerator.cpp
  PUBLIC NodePositionParticleGenerator.hpp
  PRIVATE OrbitPositionParticleGenerator.cpp
  PUBLIC OrbitPositionParticleGenerator.hpp
  PUBLIC RandomValueParticleGenerator.hpp
  PRIVATE SpherePositionParticleGenerator.cpp
  PUBLIC SpherePositionParticleGenerator.hpp
  PRIVATE SphereVelocityParticleGenerator.cpp
  PUBLIC SphereVelocityParticleGenerator.hpp
  PRIVATE TimeParticleGenerator.cpp
  PUBLIC TimeParticleGenerator.hpp
  PRIVATE UniformScaleParticleGenerator.cpp
  PUBLIC UniformScaleParticleGenerator.hpp
  PRIVATE VelocityParticleGenerator.cpp
  PUBLIC VelocityParticleGenerator.hpp
)
