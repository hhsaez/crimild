crimild_trace()

set(
    CRIMILD_TEST_NAME
    crimild_glfw_test
)

set( 
    CRIMILD_TEST_SOURCE_FILES 
	Coding/JSONCodingTest.cpp
    Editor/EditorProjectTest.cpp
	TestRunner.cpp
)

add_executable( 
    ${CRIMILD_TEST_NAME} 
    ${CRIMILD_TEST_SOURCE_FILES}
)

target_include_directories( 
	${CRIMILD_TEST_NAME} 
	PRIVATE . 
	PRIVATE ${Crimild_SOURCE_DIR}/third-party/json/include
)

target_link_libraries( 
    ${CRIMILD_TEST_NAME} 
	PRIVATE Crimild::Core
    PRIVATE Crimild::Vulkan
    PRIVATE Crimild::ImGui
    PRIVATE Crimild::Import
    PRIVATE Crimild::GLFW
    PRIVATE gtest gmock
)

gtest_discover_tests( 
    ${CRIMILD_TEST_NAME}
    WORKING_DIRECTORY ${PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)

set_target_properties( ${CRIMILD_TEST_NAME} PROPERTIES FOLDER tests )
