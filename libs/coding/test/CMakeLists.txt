add_executable( crimild_coding_test )

target_sources( 
  crimild_coding_test

  PRIVATE CodableTest.cpp
  PRIVATE MemoryEncoderTest.cpp
  PRIVATE TextCodingTest.cpp
  PRIVATE TextDecoderTest.cpp
  PRIVATE TextEncoderTest.cpp

  PRIVATE TestRunner.cpp
)

target_include_directories(
  crimild_coding_test
  PRIVATE .
  PRIVATE ../src
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
