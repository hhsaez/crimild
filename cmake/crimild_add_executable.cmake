function( CRIMILD_ADD_EXECUTABLE CRIMILD_EXEC_NAME CRIMILD_EXEC_SOURCE_FILES )
    add_executable( 
        ${CRIMILD_EXEC_NAME} 
        ${CRIMILD_EXEC_SOURCE_FILES}
    )

    target_include_directories( 
        ${CRIMILD_EXEC_NAME} 
        PRIVATE . 
    )

    target_link_libraries( 
        ${CRIMILD_EXEC_NAME} 
        PRIVATE crimild_core
    )

    target_compile_features( ${CRIMILD_EXEC_NAME} PUBLIC cxx_std_20 )
endfunction()

