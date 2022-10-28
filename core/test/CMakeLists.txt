crimild_trace()

set(
    CRIMILD_TEST_NAME
    crimild_core_test
)

set( 
    CRIMILD_TEST_SOURCE_FILES 
    Behaviors/Actions/MotionApplyTest.cpp
    Behaviors/Actions/MotionFromInputTest.cpp
    Behaviors/Actions/MotionResetTest.cpp
    Behaviors/Actions/RotateTest.cpp
    Behaviors/BehaviorContextTest.cpp
    Behaviors/BehaviorControllerTest.cpp
    Behaviors/BehaviorTreeTest.cpp
    Boundings/AABBBoundingVolumeTest.cpp
    Coding/CodableTest.cpp
    Components/MaterialComponentTest.cpp
    Components/MotionStateComponentTest.cpp
    Foundation/Containers/ArrayTest.cpp
    Foundation/Containers/DigraphTest.cpp
    Foundation/Containers/MapTest.cpp
    Foundation/Containers/PriorityQueueTest.cpp
    Foundation/Containers/QueueTest.cpp
    Foundation/Containers/SetTest.cpp
    Foundation/Containers/StackTest.cpp
    Foundation/NamedObjectTest.cpp
    Foundation/Policies/PriorityTest.cpp
    Foundation/RTTITest.cpp
    Foundation/VariantTest.cpp
    Foundation/VersionTest.cpp
    Mathematics/Bounds3Test.cpp
    Mathematics/BoxTest.cpp
    Mathematics/ClockTest.cpp
    Mathematics/ColorRGBATest.cpp
    Mathematics/ColorRGBTest.cpp
    Mathematics/CylinderTest.cpp
    Mathematics/intersectTest.cpp
    Mathematics/Matrix3Test.cpp
    Mathematics/Matrix4Test.cpp
    Mathematics/max.cpp
    Mathematics/min.cpp
    Mathematics/Normal3Test.cpp
    Mathematics/Plane3Test.cpp
    Mathematics/Point3Test.cpp
    Mathematics/QuaternionTest.cpp
    Mathematics/Ray3Test.cpp
    Mathematics/RectTest.cpp
    Mathematics/reflectTest.cpp
    Mathematics/roundTest.cpp
    Mathematics/SphereTest.cpp
    Mathematics/TraitsTest.cpp
    Mathematics/TransformationTest.cpp
    Mathematics/TriangleTest.cpp
    Mathematics/UtilsTest.cpp
    Mathematics/Vector2Test.cpp
    Mathematics/Vector3Test.cpp
    Mathematics/Vector4Test.cpp
    Messaging/MessageQueueTest.cpp
    Primitives/PrimitiveTest.cpp
    Primitives/QuadPrimitiveTest.cpp
    Rendering/AttachmentTest.cpp
    Rendering/BufferAccessorTest.cpp
    Rendering/BufferTest.cpp
    Rendering/BufferViewTest.cpp
    Rendering/CameraTest.cpp
    Rendering/CommandBufferTest.cpp
    Rendering/DescriptorSetTest.cpp
    Rendering/ImageTest.cpp
    Rendering/ImageViewTest.cpp
    Rendering/IndexBufferTest.cpp
    Rendering/Materials
    Rendering/Materials/PrincipledBSDFMaterialTest.cpp
    Rendering/Materials/UnlitMaterialTest.cpp
    Rendering/MaterialTest.cpp
    Rendering/PipelineTest.cpp
    Rendering/RenderPassTest.cpp
    Rendering/SamplerTest.cpp
    Rendering/ShaderProgramTest.cpp
    Rendering/ShaderTest.cpp
    Rendering/SkinnedMeshTest.cpp
    Rendering/TextureTest.cpp
    Rendering/UniformBufferTest.cpp
    Rendering/VertexBufferTest.cpp
    Rendering/VertexLayoutTest.cpp
    Rendering/VertexTest.cpp
    SceneGraph/GeometryTest.cpp
    SceneGraph/GroupTest.cpp
    SceneGraph/LightTest.cpp
    SceneGraph/NodeTest.cpp
    Simulation/InputTest.cpp
    Simulation/SimulationTest.cpp
    TestRunner.cpp
    Utils/MockComponent.hpp
    Utils/MockMessageHandler.hpp
    Utils/MockVisitor.hpp
    Visitors/BinTreeSceneTest.cpp
    Visitors/IntersectWorldTest.cpp
    Visitors/NodeVisitorTest.cpp
    Visitors/RTAccelerationTest.cpp
    Visitors/UpdateRenderStateTest.cpp
    Visitors/UpdateWorldStateTest.cpp
)

add_executable( 
    crimild_core_test 
    ${CRIMILD_TEST_SOURCE_FILES}
)

target_include_directories( ${CRIMILD_TEST_NAME} PRIVATE . )

target_link_libraries( 
    ${CRIMILD_TEST_NAME} 
    PRIVATE crimild_core 
    PRIVATE gtest gmock
)

gtest_discover_tests( 
    ${CRIMILD_TEST_NAME}
    WORKING_DIRECTORY ${PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)

set_target_properties( ${CRIMILD_TEST_NAME} PROPERTIES FOLDER tests )
