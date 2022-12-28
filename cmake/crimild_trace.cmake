function( CRIMILD_TRACE )
    string( REPLACE ${PROJECT_SOURCE_DIR} "crimild" RELATIVE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
    message( "Configuring: ${RELATIVE_SOURCE_DIR}" )
endfunction()

