add_executable( crimild_coding_test )

target_sources(
  crimild_coding_test
  PRIVATE
    ../src/crimild/coding/Codable.test.cpp
    ../src/crimild/coding/MemoryEncoder.test.cpp
    ../src/crimild/coding/TextCoding.test.cpp
    ../src/crimild/coding/TextDecoder.test.cpp
    ../src/crimild/coding/TextEncoder.test.cpp
    TestRunner.cpp
)

target_link_libraries(
  crimild_coding_test
  PRIVATE crimild::foundation
  PRIVATE crimild::coding
  PRIVATE gtest gmock
)

gtest_discover_tests(
  crimild_coding_test
  WORKING_DIRECTORY ${PROJECT_DIR}
  PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)

set_target_properties( crimild_coding_test PROPERTIES FOLDER tests )
