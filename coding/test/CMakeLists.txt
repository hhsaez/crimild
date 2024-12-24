crimild_trace()

add_executable( crimild_coding_test )

target_sources(
    crimild_coding_test

    PRIVATE
    CodableTest.cpp

    PRIVATE
    TestRunner.cpp
)

target_include_directories( crimild_coding_test PRIVATE . )

target_compile_features( crimild_coding_test PUBLIC cxx_std_20 )

target_link_libraries(
    crimild_coding_test
    PRIVATE Crimild::Coding
    PRIVATE Crimild::Foundation
    PRIVATE gtest gmock
)

gtest_discover_tests(
    crimild_coding_test
    WORKING_DIRECTORY ${PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)

set_target_properties( crimild_coding_test PROPERTIES FOLDER tests )
