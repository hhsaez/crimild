target_sources(
  crimild_coding_test

  PRIVATE Codable.test.cpp
  PRIVATE MemoryEncoder.test.cpp
  PRIVATE TextCoding.test.cpp
  PRIVATE TextDecoder.test.cpp
  PRIVATE TextEncoder.test.cpp
  PRIVATE TestRunner.cpp
)

# Tests also exercise the internal coding tags.
target_include_directories(
  crimild_coding_test
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/crimild/coding
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
