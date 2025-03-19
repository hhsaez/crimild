crimild_trace()

add_executable( crimild_next_test )

target_sources(
    crimild_next_test

    PRIVATE
    TestRunner.cpp

    PRIVATE
    Object.test.cpp
)

target_include_directories( crimild_next_test PRIVATE . )

target_link_libraries(
    crimild_next_test
    PRIVATE Crimild::Foundation
    PRIVATE Crimild::Mathematics
    PRIVATE Crimild::Coding
    PRIVATE Crimild::Next
    PRIVATE gtest gmock
)

gtest_discover_tests(
    crimild_next_test
    WORKING_DIRECTORY ${PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)

set_target_properties( crimild_next_test PROPERTIES FOLDER tests )
