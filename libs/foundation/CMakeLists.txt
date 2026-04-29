crimild_trace()

add_library( crimild_foundation OBJECT )

target_sources(
  crimild_foundation

  INTERFACE include/crimild/foundation.hpp

  PUBLIC include/crimild/foundation/common/KeyValuePair.hpp
  PUBLIC include/crimild/foundation/common/Macros.hpp
  PUBLIC include/crimild/foundation/common/NamedObject.hpp
  PUBLIC include/crimild/foundation/common/ObjectFactory.hpp
  PUBLIC include/crimild/foundation/common/Observable.hpp
  PUBLIC include/crimild/foundation/common/RTTI.hpp
  PUBLIC include/crimild/foundation/common/SharedObject.hpp
  PUBLIC include/crimild/foundation/common/Singleton.hpp
  PUBLIC include/crimild/foundation/common/StringUtils.hpp
  PUBLIC include/crimild/foundation/common/Types.hpp
  PUBLIC include/crimild/foundation/common/Version.hpp

  PUBLIC include/crimild/foundation/containers/Array.hpp
  PUBLIC include/crimild/foundation/containers/Digraph.hpp
  PUBLIC include/crimild/foundation/containers/List.hpp
  PUBLIC include/crimild/foundation/containers/Map.hpp
  PUBLIC include/crimild/foundation/containers/PriorityQueue.hpp
  PUBLIC include/crimild/foundation/containers/Queue.hpp
  PUBLIC include/crimild/foundation/containers/Set.hpp
  PUBLIC include/crimild/foundation/containers/Stack.hpp

  PUBLIC include/crimild/foundation/filesystem/FilePath.hpp
  
  PUBLIC include/crimild/foundation/log/Log.hpp
  
  PUBLIC include/crimild/foundation/memory/Chunk.hpp
  PUBLIC include/crimild/foundation/memory/FixedAllocator.hpp
  PUBLIC include/crimild/foundation/memory/Memory.hpp
  PUBLIC include/crimild/foundation/memory/SmallObject.hpp
  PUBLIC include/crimild/foundation/memory/SmallObjectAllocator.hpp

  PUBLIC include/crimild/foundation/policies/NonCopyable.hpp
  PUBLIC include/crimild/foundation/policies/CachePolicy.hpp
  PUBLIC include/crimild/foundation/policies/ThreadingPolicy.hpp

  PRIVATE src/common/NamedObject.cpp
  PRIVATE src/common/ObjectFactory.cpp
  PRIVATE src/common/Version.cpp

  PRIVATE src/filesystem/FilePath.cpp

  PRIVATE src/log/Log.cpp

  PRIVATE src/memory/Chunk.cpp
  PRIVATE src/memory/FixedAllocator.cpp
  PRIVATE src/memory/SmallObjectAllocator.cpp
)

target_include_directories(
  crimild_foundation
  PUBLIC include
  PRIVATE src
)

target_compile_features( crimild_foundation PUBLIC cxx_std_20 )

add_library( crimild::foundation ALIAS crimild_foundation )

add_subdirectory( test )
