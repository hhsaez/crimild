crimild_trace()

add_library( crimild_foundation STATIC )

target_include_directories(
  crimild_foundation
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_features( crimild_foundation PUBLIC cxx_std_20 )

add_library( crimild::foundation ALIAS crimild_foundation )

if ( CRIMILD_BUILD_TESTS )
  add_subdirectory( test )
endif ()

add_subdirectory( src )
