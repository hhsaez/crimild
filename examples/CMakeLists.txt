crimild_trace()

function( CRIMILD_ADD_EXAMPLE EXAMPLE_NAME EXAMPLE_SOURCES )
    add_executable( ${EXAMPLE_NAME} )
    target_sources( ${EXAMPLE_NAME} PRIVATE ${EXAMPLE_SOURCES} )
    target_include_directories( ${EXAMPLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )
    target_link_libraries(
        ${EXAMPLE_NAME}
        PRIVATE crimild::foundation
        PRIVATE Crimild::Mathematics
        PRIVATE crimild::coding
        PRIVATE Crimild::Core
        PRIVATE Crimild::Vulkan
        PRIVATE Crimild::Player
    )
    set_target_properties( ${EXAMPLE_NAME} PROPERTIES FOLDER examples )
endfunction()

if ( CRIMILD_BUILD_EXAMPLES )
  add_subdirectory( LightingDirectional )
  add_subdirectory( LightingMulitple )
  add_subdirectory( LightingPoint )
  add_subdirectory( LightingSpotlight )
  add_subdirectory( LightingUnlit )
  add_subdirectory( Playground )
  # add_subdirectory( PBRBasic )
  add_subdirectory( ShadowsDirectional )
  add_subdirectory( ShadowsDirectionalMultiple )
  add_subdirectory( ShadowsSpot )
  add_subdirectory( ShadowsSpotMultiple )
  add_subdirectory( ShadowsPoint )
  add_subdirectory( ShadowsPointMultiple )
  add_subdirectory( TextureCoordinates )
  add_subdirectory( Triangle )
endif()

if ( CRIMILD_BUILD_UNIVERSAL_EXAMPLES )
  add_subdirectory( Triangle3D )
  add_subdirectory( Bounding3D )
endif() 

