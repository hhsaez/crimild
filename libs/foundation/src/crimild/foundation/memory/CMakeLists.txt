target_sources(
  crimild_foundation
  PUBLIC Chunk.hpp
  PUBLIC FixedAllocator.hpp
  PUBLIC Memory.hpp
  PUBLIC SmallObject.hpp
  PUBLIC SmallObjectAllocator.hpp
  PRIVATE Chunk.cpp
  PRIVATE FixedAllocator.cpp
  PRIVATE SmallObjectAllocator.cpp
)

