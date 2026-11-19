target_sources(
  crimild_core

  PRIVATE ArcPrimitive.cpp
  PUBLIC ArcPrimitive.hpp
  PRIVATE ArrowPrimitive.cpp
  PUBLIC ArrowPrimitive.hpp
  PRIVATE BoxPrimitive.cpp
  PUBLIC BoxPrimitive.hpp
  PRIVATE ConePrimitive.cpp
  PUBLIC ConePrimitive.hpp
  PRIVATE CylinderPrimitive.cpp
  PUBLIC CylinderPrimitive.hpp
  PRIVATE KleinBottlePrimitive.cpp
  PUBLIC KleinBottlePrimitive.hpp
  PRIVATE MobiusStripPrimitive.cpp
  PUBLIC MobiusStripPrimitive.hpp
  PRIVATE NewellTeapotPrimitive.cpp
  PUBLIC NewellTeapotPrimitive.hpp
  PRIVATE ParametricPrimitive.cpp
  PUBLIC ParametricPrimitive.hpp
  PRIVATE ParametricSpherePrimitive.cpp
  PUBLIC ParametricSpherePrimitive.hpp
  PRIVATE Primitive.cpp
  PUBLIC Primitive.hpp
  PRIVATE QuadPrimitive.cpp
  PUBLIC QuadPrimitive.hpp
  PRIVATE SpherePrimitive.cpp
  PUBLIC SpherePrimitive.hpp
  PRIVATE TorusPrimitive.cpp
  PUBLIC TorusPrimitive.hpp
  PRIVATE TrefoilKnotPrimitive.cpp
  PUBLIC TrefoilKnotPrimitive.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE Primitive.test.cpp
    PRIVATE QuadPrimitive.test.cpp
  )
endif ()
