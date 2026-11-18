crimild_trace()

add_executable( crimild_core_test )

target_sources(
  crimild_core_test

  PRIVATE TestRunner.cpp
  PRIVATE Utils/MockComponent.hpp
  PRIVATE Utils/MockMessageHandler.hpp
  PRIVATE Utils/MockVisitor.hpp
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
