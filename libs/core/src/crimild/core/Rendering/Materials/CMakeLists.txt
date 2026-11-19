target_sources(
  crimild_core

  PRIVATE PrincipledBSDFMaterial.cpp
  PUBLIC PrincipledBSDFMaterial.hpp
  PRIVATE PrincipledVolumeMaterial.cpp
  PUBLIC PrincipledVolumeMaterial.hpp
  PRIVATE SkyboxMaterial.cpp
  PUBLIC SkyboxMaterial.hpp
  PRIVATE UnlitMaterial.cpp
  PUBLIC UnlitMaterial.hpp
  PRIVATE WorldGridMaterial.cpp
  PUBLIC WorldGridMaterial.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE PrincipledBSDFMaterial.test.cpp
    PRIVATE UnlitMaterial.test.cpp
  )
endif ()
