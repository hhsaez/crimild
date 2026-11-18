target_sources(
  crimild_core

  PRIVATE MessageQueue.cpp
  PUBLIC MessageQueue.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE MessageQueue.test.cpp
  )
endif ()
