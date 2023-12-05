crimild_trace()

add_library( crimild_foundation OBJECT )

target_sources( 
    crimild_foundation
    
    PUBLIC
    Crimild_Foundation.hpp

    PRIVATE
    Common/KeyValuePair.hpp
    Common/Macros.hpp
    Common/NamedObject.hpp
    Common/ObjectFactory.hpp
    Common/Observable.hpp
    Common/RTTI.hpp
    Common/SharedObject.hpp
    Common/Singleton.hpp
    Common/StringUtils.hpp
    Common/Types.hpp
    Common/Version.hpp
    Containers/Array.hpp
    Containers/Digraph.hpp
    Containers/List.hpp
    Containers/Map.hpp
    Containers/PriorityQueue.hpp
    Containers/Queue.hpp
    Containers/Set.hpp
    Containers/Stack.hpp
    FileSystem/FilePath.hpp
    Log/Log.hpp
    Memory/Chunk.hpp
    Memory/FixedAllocator.hpp
    Memory/Memory.hpp
    Memory/SmallObject.hpp
    Memory/SmallObjectAllocator.hpp
    Policies/NonCopyable.hpp
    Policies/CachePolicy.hpp
    Policies/ThreadingPolicy.hpp

    PRIVATE
    Crimild_Foundation.cpp
    Common/NamedObject.cpp
    Common/ObjectFactory.cpp
    Common/Version.cpp
    FileSystem/FilePath.cpp
    Log/Log.cpp
    Memory/Chunk.cpp
    Memory/FixedAllocator.cpp
    Memory/SmallObjectAllocator.cpp
)

target_include_directories( crimild_foundation PUBLIC . )
target_compile_features( crimild_foundation PUBLIC cxx_std_20 )
add_library( Crimild::Foundation ALIAS crimild_foundation )

