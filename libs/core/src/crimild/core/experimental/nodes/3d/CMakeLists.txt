target_sources(
  crimild_core

  PUBLIC BoundingVolume3D.hpp
  PRIVATE Camera3D.cpp
  PUBLIC Camera3D.hpp
  PRIVATE Geometry3D.cpp
  PUBLIC Geometry3D.hpp
  PRIVATE Spatial3D.cpp
  PUBLIC Spatial3D.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE Geometry3D.test.cpp
    PRIVATE Spatial3D.test.cpp
  )
endif ()
