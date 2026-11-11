crimild_trace()

add_executable( crimild_core_test )

target_sources(
    crimild_core_test

    PRIVATE experimental/behaviors/Behavior.test.cpp
    PRIVATE experimental/behaviors/Composite.test.cpp
    PRIVATE experimental/behaviors/Decorator.test.cpp
    PRIVATE experimental/behaviors/Sequence.test.cpp

    PRIVATE Common/Signal.test.cpp

    PRIVATE Assemblies/Assembly.test.cpp

    PRIVATE Nodes/Node.test.cpp

    PRIVATE Nodes/3D/Geometry3D.test.cpp
    PRIVATE Nodes/3D/Spatial3D.test.cpp
    
    PRIVATE Behaviors/Actions/MotionApplyTest.cpp
    PRIVATE Behaviors/Actions/MotionFromInputTest.cpp
    PRIVATE Behaviors/Actions/MotionResetTest.cpp
    PRIVATE Behaviors/Actions/RotateTest.cpp
    PRIVATE Behaviors/BehaviorContextTest.cpp
    PRIVATE Behaviors/BehaviorControllerTest.cpp
    PRIVATE Behaviors/BehaviorTreeTest.cpp
    PRIVATE Boundings/AABBBoundingVolumeTest.cpp
    PRIVATE Components/MaterialComponentTest.cpp
    PRIVATE Components/MotionStateComponentTest.cpp
    PRIVATE Entity/Entity.test.cpp
    PRIVATE Messaging/MessageQueueTest.cpp
    PRIVATE Primitives/PrimitiveTest.cpp
    PRIVATE Primitives/QuadPrimitiveTest.cpp
    PRIVATE Rendering/AttachmentTest.cpp
    PRIVATE Rendering/BufferAccessorTest.cpp
    PRIVATE Rendering/BufferTest.cpp
    PRIVATE Rendering/BufferViewTest.cpp
    PRIVATE Rendering/CameraTest.cpp
    PRIVATE Rendering/CommandBufferTest.cpp
    PRIVATE Rendering/DescriptorSetTest.cpp
    PRIVATE Rendering/ImageTest.cpp
    PRIVATE Rendering/ImageViewTest.cpp
    PRIVATE Rendering/IndexBufferTest.cpp
    PRIVATE Rendering/Materials/PrincipledBSDFMaterialTest.cpp
    PRIVATE Rendering/Materials/UnlitMaterialTest.cpp
    PRIVATE Rendering/MaterialTest.cpp
    PRIVATE Rendering/PipelineTest.cpp
    PRIVATE Rendering/RenderPassTest.cpp
    PRIVATE Rendering/SamplerTest.cpp
    PRIVATE Rendering/ShaderProgramTest.cpp
    PRIVATE Rendering/ShaderTest.cpp
    PRIVATE Rendering/SkinnedMeshTest.cpp
    PRIVATE Rendering/TextureTest.cpp
    PRIVATE Rendering/UniformBufferTest.cpp
    PRIVATE Rendering/VertexAttributeTest.cpp
    PRIVATE Rendering/VertexBufferTest.cpp
    PRIVATE Rendering/VertexLayoutTest.cpp
    PRIVATE Rendering/VertexTest.cpp
    PRIVATE SceneGraph/GeometryTest.cpp
    PRIVATE SceneGraph/GroupTest.cpp
    PRIVATE SceneGraph/LightTest.cpp
    PRIVATE SceneGraph/NodeTest.cpp
    PRIVATE Simulation/InputTest.cpp
    PRIVATE Simulation/SimulationTest.cpp
    PRIVATE TestRunner.cpp
    PRIVATE Utils/MockComponent.hpp
    PRIVATE Utils/MockMessageHandler.hpp
    PRIVATE Utils/MockVisitor.hpp
    PRIVATE Visitors/BinTreeSceneTest.cpp
    PRIVATE Visitors/IntersectWorldTest.cpp
    PRIVATE Visitors/NodeVisitorTest.cpp
    PRIVATE Visitors/RTAccelerationTest.cpp
    PRIVATE Visitors/UpdateRenderStateTest.cpp
    PRIVATE Visitors/UpdateWorldStateTest.cpp
)

target_include_directories(
    crimild_core_test

    PRIVATE .
)

target_link_libraries(
    crimild_core_test
    PRIVATE crimild::core
    PRIVATE gtest gmock
)

gtest_discover_tests(
    crimild_core_test
    WORKING_DIRECTORY ${PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)

set_target_properties( crimild_core_test PROPERTIES FOLDER tests )
