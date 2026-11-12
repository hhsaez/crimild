crimild_trace()

add_executable( crimild_math_test )

target_sources(
    crimild_math_test

    PRIVATE Bounds3Test.cpp
    PRIVATE BoxTest.cpp
    PRIVATE ColorRGBATest.cpp
    PRIVATE ColorRGBTest.cpp
    PRIVATE CylinderTest.cpp
    PRIVATE intersectTest.cpp
    PRIVATE Matrix3Test.cpp
    PRIVATE Matrix4Test.cpp
    PRIVATE maxTest.cpp
    PRIVATE minTest.cpp
    PRIVATE Normal3Test.cpp
    PRIVATE Plane3Test.cpp
    PRIVATE Point2Test.cpp
    PRIVATE Point3Test.cpp
    PRIVATE QuaternionTest.cpp
    PRIVATE Ray3Test.cpp
    PRIVATE RectTest.cpp
    PRIVATE reflectTest.cpp
    PRIVATE roundTest.cpp
    PRIVATE SphereTest.cpp
    PRIVATE TransformationTest.cpp
    PRIVATE TriangleTest.cpp
    PRIVATE UtilsTest.cpp
    PRIVATE Vector2Test.cpp
    PRIVATE Vector3Test.cpp
    PRIVATE Vector4Test.cpp

    PRIVATE TestRunner.cpp
)

target_include_directories(
  crimild_math_test
  PRIVATE ../src
)

target_compile_features( crimild_math_test PUBLIC cxx_std_20 )

target_link_libraries(
    crimild_math_test

    PRIVATE crimild::math
    PRIVATE gtest gmock
)

gtest_discover_tests(
    crimild_math_test
    WORKING_DIRECTORY ${PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)

set_target_properties( crimild_math_test PROPERTIES FOLDER tests )
