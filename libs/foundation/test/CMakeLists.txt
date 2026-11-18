add_executable( crimild_foundation_test )

target_sources(
  crimild_foundation_test
  PRIVATE TestRunner.cpp
)

target_link_libraries(
  crimild_foundation_test
  PRIVATE crimild::foundation
  PRIVATE gtest gmock
)

gtest_discover_tests(
  crimild_foundation_test
  WORKING_DIRECTORY ${PROJECT_DIR}
  PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)

set_target_properties( crimild_foundation_test PROPERTIES FOLDER tests )
