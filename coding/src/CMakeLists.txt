crimild_trace()

add_library( crimild_coding OBJECT )

target_sources(
    crimild_coding

    PUBLIC Crimild_Coding.hpp

    PRIVATE
    Codable.hpp
    Decoder.hpp
    EncodedData.hpp
    EncodedObject.hpp
    Encoder.hpp
    FileDecoder.hpp
    FileEncoder.hpp
    MemoryDecoder.hpp
    MemoryEncoder.hpp
    Tags.hpp

    PRIVATE
    Crimild_Coding.cpp

    PRIVATE
    FileDecoder.cpp
    FileEncoder.cpp
    MemoryDecoder.cpp
    MemoryEncoder.cpp
    Tags.cpp
)

target_include_directories( crimild_coding PUBLIC . )

target_compile_features( crimild_coding PUBLIC cxx_std_20 )

target_link_libraries(
    crimild_coding
    PUBLIC Crimild::Foundation
    PUBLIC Crimild::Mathematics
)

add_library( Crimild::Coding ALIAS crimild_coding )
