crimild_trace()

add_executable( crimild_core_test )

target_sources(
    crimild_core_test

    PRIVATE
    Behaviors/Actions/MotionApplyTest.cpp
    Behaviors/Actions/MotionFromInputTest.cpp
    Behaviors/Actions/MotionResetTest.cpp
    Behaviors/Actions/RotateTest.cpp
    Behaviors/BehaviorContextTest.cpp
    Behaviors/BehaviorControllerTest.cpp
    Behaviors/BehaviorTreeTest.cpp
    Boundings/AABBBoundingVolumeTest.cpp
    Components/MaterialComponentTest.cpp
    Components/MotionStateComponentTest.cpp
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
    Rendering/VertexAttributeTest.cpp
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

target_include_directories( crimild_core_test PRIVATE . )

target_link_libraries(
    crimild_core_test
    PRIVATE Crimild::Foundation
    PRIVATE Crimild::Mathematics
    PRIVATE Crimild::Coding
    PRIVATE Crimild::Core
    PRIVATE gtest gmock
)

gtest_discover_tests(
    crimild_core_test
    WORKING_DIRECTORY ${PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)

set_target_properties( crimild_core_test PROPERTIES FOLDER tests )
