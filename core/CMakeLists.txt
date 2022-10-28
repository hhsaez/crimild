# MESSAGE( "-- Configuring core library" )

# SET( CRIMILD_LIBRARY_NAME core )

# ADD_SUBDIRECTORY( src )

# IF ( CRIMILD_ENABLE_TESTS )
# 	ADD_SUBDIRECTORY( test )
# ENDIF ( CRIMILD_ENABLE_TESTS )

crimild_trace()

add_subdirectory( src )
if ( CRIMILD_BUILD_TESTS )
	add_subdirectory( test )
endif ()
