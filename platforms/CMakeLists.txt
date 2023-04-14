crimild_trace()

if( CRIMILD_BUILD_EDITOR )
    add_subdirectory( Editor )
endif()

if( CRIMILD_BUILD_PLAYER )
    add_subdirectory( Player )
endif()
