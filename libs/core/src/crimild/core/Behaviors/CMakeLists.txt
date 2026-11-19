target_sources(
  crimild_core

  PRIVATE Behavior.cpp
  PUBLIC Behavior.hpp
  PRIVATE BehaviorContext.cpp
  PUBLIC BehaviorContext.hpp
  PRIVATE BehaviorController.cpp
  PUBLIC BehaviorController.hpp
  PRIVATE BehaviorTree.cpp
  PUBLIC BehaviorTree.hpp
  PUBLIC withBehavior.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE BehaviorContext.test.cpp
    PRIVATE BehaviorController.test.cpp
    PRIVATE BehaviorTree.test.cpp
  )
endif ()

add_subdirectory( Actions )
add_subdirectory( Composites )
add_subdirectory( Conditions )
add_subdirectory( Decorators )
