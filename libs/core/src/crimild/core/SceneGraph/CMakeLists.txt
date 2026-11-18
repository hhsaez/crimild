target_sources(
  crimild_core

  PRIVATE Camera.cpp
  PUBLIC Camera.hpp
  PRIVATE CSGNode.cpp
  PUBLIC CSGNode.hpp
  PRIVATE Geometry.cpp
  PUBLIC Geometry.hpp
  PRIVATE Group.cpp
  PUBLIC Group.hpp
  PRIVATE Light.cpp
  PUBLIC Light.hpp
  PUBLIC Node_io.hpp
  PUBLIC Node_withTransformation.hpp
  PRIVATE Node.cpp
  PUBLIC Node.hpp
  PUBLIC NullNode.hpp
  PRIVATE Skybox.cpp
  PUBLIC Skybox.hpp
  PRIVATE Switch.cpp
  PUBLIC Switch.hpp
  PRIVATE Text.cpp
  PUBLIC Text.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE Camera.test.cpp
    PRIVATE Geometry.test.cpp
    PRIVATE Group.test.cpp
    PRIVATE Light.test.cpp
    PRIVATE Node.test.cpp
  )
endif ()
