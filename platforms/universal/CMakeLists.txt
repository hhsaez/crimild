crimild_trace()

add_library( crimild_universal STATIC )

add_subdirectory( include )
add_subdirectory( src )

target_include_directories(
  crimild_universal

  PUBLIC include
  PRIVATE src
)

if ( WIN32 OR LINUX )
   target_include_directories(
      crimild_universal

      PRIVATE ${Crimild_SOURCE_DIR}/third-party/glad/include
   )
endif()

target_link_libraries(
   crimild_universal

   PRIVATE crimild::foundation
   PRIVATE crimild::math
   PRIVATE crimild::coding
   PRIVATE crimild::core
   PRIVATE glfw
)

if (APPLE)
   target_link_libraries(
      crimild_universal

      PRIVATE "-framework OpenGL"
   )
endif()

add_library( crimild::universal ALIAS crimild_universal )
