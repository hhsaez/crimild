target_sources(
  crimild_core

  PRIVATE Entity.cpp
  PUBLIC Entity.hpp
  PUBLIC Extension.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE Entity.test.cpp
  )
endif ()
