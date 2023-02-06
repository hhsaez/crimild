crimild_trace()

function( CRIMILD_ADD_EXAMPLE EXAMPLE_NAME EXAMPLE_SOURCES )
    add_executable( ${EXAMPLE_NAME} )
    target_sources( ${EXAMPLE_NAME} PRIVATE ${EXAMPLE_SOURCES} )
    target_include_directories( ${EXAMPLE_NAME} PRIVATE . )
    target_link_libraries(
        ${EXAMPLE_NAME}
        PRIVATE Crimild::Core
        PRIVATE Crimild::Vulkan
        PRIVATE Crimild::ImGui
        PRIVATE Crimild::Import
        PRIVATE Crimild::GLFW
        PRIVATE Crimild::GLFW::Main
    )
    set_target_properties( ${EXAMPLE_NAME} PROPERTIES FOLDER examples )
endfunction()

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
