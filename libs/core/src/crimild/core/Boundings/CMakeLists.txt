target_sources(
  crimild_core

  PRIVATE AABBBoundingVolume.cpp
  PUBLIC AABBBoundingVolume.hpp
  PRIVATE BoundingVolume.cpp
  PUBLIC BoundingVolume.hpp
  PRIVATE Box2DBoundingVolume.cpp
  PUBLIC Box2DBoundingVolume.hpp
  PRIVATE OrientedBoxBoundingVolume.cpp
  PUBLIC OrientedBoxBoundingVolume.hpp
  PRIVATE PlaneBoundingVolume.cpp
  PUBLIC PlaneBoundingVolume.hpp
  PRIVATE SphereBoundingVolume.cpp
  PUBLIC SphereBoundingVolume.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE AABBBoundingVolume.test.cpp
  )
endif ()
