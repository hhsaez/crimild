target_sources(
  crimild_core
  PUBLIC Blackboard.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test
    PRIVATE Blackboard.test.cpp
  )
endif ()

add_subdirectory( behaviors )
add_subdirectory( nodes )
add_subdirectory( systems )
