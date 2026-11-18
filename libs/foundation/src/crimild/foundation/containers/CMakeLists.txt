target_sources(
  crimild_foundation
  PUBLIC Array.hpp
  PUBLIC Digraph.hpp
  PUBLIC List.hpp
  PUBLIC Map.hpp
  PUBLIC PriorityQueue.hpp
  PUBLIC Queue.hpp
  PUBLIC Set.hpp
  PUBLIC Stack.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_foundation_test
    PRIVATE Array.test.cpp
    PRIVATE Digraph.test.cpp
    PRIVATE Map.test.cpp
    PRIVATE PriorityQueue.test.cpp
    PRIVATE Queue.test.cpp
    PRIVATE Set.test.cpp
    PRIVATE Stack.test.cpp
  )
endif ()

