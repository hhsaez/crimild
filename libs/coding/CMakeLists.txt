crimild_trace()

add_library( crimild_coding OBJECT )

target_sources(
  crimild_coding

  INTERFACE include/crimild/coding.hpp

  PUBLIC include/crimild/coding/Codable.hpp
  PUBLIC include/crimild/coding/EncodedData.hpp
  PUBLIC include/crimild/coding/Encoder.hpp
  PUBLIC include/crimild/coding/Decoder.hpp
  PUBLIC include/crimild/coding/FileEncoder.hpp
  PUBLIC include/crimild/coding/FileDecoder.hpp
  PUBLIC include/crimild/coding/init.hpp
  PUBLIC include/crimild/coding/MemoryEncoder.hpp
  PUBLIC include/crimild/coding/MemoryDecoder.hpp
  PUBLIC include/crimild/coding/TextEncoder.hpp
  PUBLIC include/crimild/coding/TextDecoder.hpp

  PRIVATE src/EncodedObject.hpp
  PRIVATE src/FileDecoder.cpp
  PRIVATE src/FileEncoder.cpp
  PRIVATE src/init.cpp
  PRIVATE src/MemoryDecoder.cpp
  PRIVATE src/MemoryEncoder.cpp
  PRIVATE src/Tags.cpp
  PRIVATE src/Tags.hpp
  PRIVATE src/TextDecoder.cpp
  PRIVATE src/TextEncoder.cpp
)

target_include_directories(
  crimild_coding
  PUBLIC include
  PRIVATE src
)

target_link_libraries(
    crimild_coding
    PUBLIC crimild::foundation
    PUBLIC Crimild::Mathematics
)

target_compile_features( crimild_coding PUBLIC cxx_std_20 )

add_library( crimild::coding ALIAS crimild_coding )

add_subdirectory( test )
