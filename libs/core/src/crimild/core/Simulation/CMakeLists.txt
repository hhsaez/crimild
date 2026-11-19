target_sources(
  crimild_core

  PRIVATE AssetManager.cpp
  PUBLIC AssetManager.hpp
  PRIVATE Clock.cpp
  PUBLIC Clock.hpp
  PUBLIC Event.hpp
  PRIVATE FileSystem.cpp
  PUBLIC FileSystem.hpp
  PRIVATE Input.cpp
  PUBLIC Input.hpp
  PRIVATE Settings.cpp
  PUBLIC Settings.hpp
  PRIVATE Simulation.cpp
  PUBLIC Simulation.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE Input.test.cpp
    PRIVATE Simulation.test.cpp
  )
endif ()

add_subdirectory( Console )
add_subdirectory( Systems )
