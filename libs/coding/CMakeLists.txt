crimild_trace()

add_library( crimild_coding STATIC )

target_sources(
  crimild_coding
  PRIVATE
    src/crimild/coding.hpp
    src/crimild/coding/Codable.hpp
    src/crimild/coding/Decoder.hpp
    src/crimild/coding/EncodedData.hpp
    src/crimild/coding/Encoder.hpp
    src/crimild/coding/FileDecoder.hpp
    src/crimild/coding/FileEncoder.hpp
    src/crimild/coding/MemoryDecoder.hpp
    src/crimild/coding/MemoryEncoder.hpp
    src/crimild/coding/TextDecoder.hpp
    src/crimild/coding/TextEncoder.hpp
    src/crimild/coding/init.hpp
    src/crimild/coding/EncodedObject.hpp
    src/crimild/coding/FileDecoder.cpp
    src/crimild/coding/FileEncoder.cpp
    src/crimild/coding/MemoryDecoder.cpp
    src/crimild/coding/MemoryEncoder.cpp
    src/crimild/coding/Tags.cpp
    src/crimild/coding/Tags.hpp
    src/crimild/coding/TextDecoder.cpp
    src/crimild/coding/TextEncoder.cpp
    src/crimild/coding/init.cpp
)

target_include_directories(
  crimild_coding
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(
  crimild_coding
  PUBLIC crimild::foundation
  PUBLIC crimild::math
)

target_compile_features( crimild_coding PUBLIC cxx_std_20 )

add_library( crimild::coding ALIAS crimild_coding )

if ( CRIMILD_BUILD_TESTS )
  add_subdirectory( test )
endif ()
