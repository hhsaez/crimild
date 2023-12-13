crimild_trace()

add_executable( crimild_mathematics_test )

target_sources( 
    crimild_mathematics_test

    PRIVATE
    Bounds3Test.cpp
    BoxTest.cpp
    ColorRGBATest.cpp
    ColorRGBTest.cpp
    CylinderTest.cpp
    intersectTest.cpp
    Matrix3Test.cpp
    Matrix4Test.cpp
    maxTest.cpp
    minTest.cpp
    Normal3Test.cpp
    Plane3Test.cpp
    Point2Test.cpp
    Point3Test.cpp
    QuaternionTest.cpp
    Ray3Test.cpp
    RectTest.cpp
    reflectTest.cpp
    roundTest.cpp
    SphereTest.cpp
    TransformationTest.cpp
    TriangleTest.cpp
    UtilsTest.cpp
    Vector2Test.cpp
    Vector3Test.cpp
    Vector4Test.cpp

    PRIVATE
    TestRunner.cpp
)

target_include_directories( crimild_mathematics_test PRIVATE . )

target_link_libraries( 
    crimild_mathematics_test
    PRIVATE Crimild::Foundation
    PRIVATE Crimild::Mathematics 
    PRIVATE gtest gmock
)

gtest_discover_tests( 
    crimild_mathematics_test
    WORKING_DIRECTORY ${PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)

set_target_properties( crimild_mathematics_test PROPERTIES FOLDER tests )
