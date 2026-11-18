crimild_trace()

add_executable( crimild_math_test )

target_sources(
  crimild_math_test
  PRIVATE TestRunner.cpp
)

target_compile_features( crimild_math_test PUBLIC cxx_std_20 )

target_link_libraries(
  crimild_math_test
  PRIVATE crimild::math
  PRIVATE gtest gmock
)

gtest_discover_tests(
  crimild_math_test
  WORKING_DIRECTORY ${PROJECT_DIR}
  PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)

set_target_properties( crimild_math_test PROPERTIES FOLDER tests )
