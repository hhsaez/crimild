target_sources(
  crimild_core

  PRIVATE AttractorParticleUpdater.cpp
  PUBLIC AttractorParticleUpdater.hpp
  PRIVATE CameraSortParticleUpdater.cpp
  PUBLIC CameraSortParticleUpdater.hpp
  PRIVATE ColorParticleUpdater.cpp
  PUBLIC ColorParticleUpdater.hpp
  PRIVATE EulerParticleUpdater.cpp
  PUBLIC EulerParticleUpdater.hpp
  PRIVATE FloorParticleUpdater.cpp
  PUBLIC FloorParticleUpdater.hpp
  PRIVATE PositionColorParticleUpdater.cpp
  PUBLIC PositionColorParticleUpdater.hpp
  PRIVATE PositionVelocityParticleUpdater.cpp
  PUBLIC PositionVelocityParticleUpdater.hpp
  PUBLIC SetAttribValueParticleUpdater.hpp
  PRIVATE TimeParticleUpdater.cpp
  PUBLIC TimeParticleUpdater.hpp
  PRIVATE UniformScaleParticleUpdater.cpp
  PUBLIC UniformScaleParticleUpdater.hpp
  PRIVATE VelocityColorParticleUpdater.cpp
  PUBLIC VelocityColorParticleUpdater.hpp
  PRIVATE ZSortParticleUpdater.cpp
  PUBLIC ZSortParticleUpdater.hpp
)
