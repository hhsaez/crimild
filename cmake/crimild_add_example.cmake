include( crimild_add_executable )

function( CRIMILD_ADD_EXAMPLE EXAMPLE_NAME EXAMPLE_SOURCE_FILES )
    crimild_add_executable( ${EXAMPLE_NAME} ${EXAMPLE_SOURCE_FILES} )

    target_link_libraries( 
        ${EXAMPLE_NAME} 
        PRIVATE crimild_glfw
    )

    set_target_properties( ${EXAMPLE_NAME} PROPERTIES FOLDER examples )
endfunction()
