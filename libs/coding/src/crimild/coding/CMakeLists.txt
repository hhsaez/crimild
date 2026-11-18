target_sources(
  crimild_coding

  PUBLIC Codable.hpp
  PUBLIC Decoder.hpp
  PUBLIC EncodedData.hpp
  PUBLIC Encoder.hpp
  PUBLIC FileDecoder.hpp
  PUBLIC FileEncoder.hpp
  PUBLIC MemoryDecoder.hpp
  PUBLIC MemoryEncoder.hpp
  PUBLIC TextDecoder.hpp
  PUBLIC TextEncoder.hpp
  PUBLIC init.hpp

  PRIVATE EncodedObject.hpp
  PRIVATE FileDecoder.cpp
  PRIVATE FileEncoder.cpp
  PRIVATE MemoryDecoder.cpp
  PRIVATE MemoryEncoder.cpp
  PRIVATE Tags.cpp
  PRIVATE Tags.hpp
  PRIVATE TextDecoder.cpp
  PRIVATE TextEncoder.cpp
  PRIVATE init.cpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_coding_test

    PRIVATE Codable.test.cpp
    PRIVATE MemoryEncoder.test.cpp
    PRIVATE TextCoding.test.cpp
    PRIVATE TextDecoder.test.cpp
    PRIVATE TextEncoder.test.cpp
    PRIVATE TestRunner.cpp
  )
endif ()
