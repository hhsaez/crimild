crimild_trace()

add_library( crimild_coding STATIC )

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

add_subdirectory( src )
