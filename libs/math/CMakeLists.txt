crimild_trace()

add_library( crimild_math STATIC )

target_include_directories(
  crimild_math
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_features( crimild_math PUBLIC cxx_std_20 )

add_library( crimild::math ALIAS crimild_math )

if ( CRIMILD_BUILD_TESTS )
  add_subdirectory( test )
endif ()

add_subdirectory( src )
