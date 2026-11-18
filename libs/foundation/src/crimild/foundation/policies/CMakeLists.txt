target_sources(
  crimild_foundation
  PUBLIC CachePolicy.hpp
  PUBLIC NonCopyable.hpp
  PUBLIC ThreadingPolicy.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_foundation_test
    PRIVATE Priority.test.cpp
  )
endif ()

