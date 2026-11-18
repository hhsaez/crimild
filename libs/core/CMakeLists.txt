crimild_trace()

add_library( crimild_core STATIC )

target_include_directories(
  crimild_core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_features( crimild_core PUBLIC cxx_std_20 )

target_link_libraries(
  crimild_core
  PUBLIC crimild::foundation
  PUBLIC crimild::math
  PUBLIC crimild::coding
)

add_library( crimild::core ALIAS crimild_core )

if ( CRIMILD_BUILD_TESTS )
  add_subdirectory( test )
endif ()

add_subdirectory( src )
