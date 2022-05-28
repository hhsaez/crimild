MESSAGE( "-- Configuring examples:" )

# TODO(Hernan): Disable all examples 
# IF ( NOT ( ${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten" ) )
#    FILE ( GLOB EXAMPLE_DIRS RELATIVE "${PROJECT_SOURCE_DIR}/examples" * )
#    FOREACH( EXAMPLE_DIR ${EXAMPLE_DIRS} )
#    		IF ( NOT ${EXAMPLE_DIR} MATCHES "CMakeFiles" )
# 		   	 IF ( IS_DIRECTORY ${PROJECT_SOURCE_DIR}/examples/${EXAMPLE_DIR} )
# 			 	ADD_SUBDIRECTORY( ${EXAMPLE_DIR} )
# 			ENDIF ( IS_DIRECTORY ${PROJECT_SOURCE_DIR}/examples/${EXAMPLE_DIR} )
# 		ENDIF ()
# 	ENDFOREACH()
# ELSE ()
# 	 ADD_SUBDIRECTORY( Triangle )
# 	 ADD_SUBDIRECTORY( Lighting )
# 	 ADD_SUBDIRECTORY( Spotlight )
# 	 ADD_SUBDIRECTORY( Shadows )
# ENDIF ()

# Only enable Playground for the moment
ADD_SUBDIRECTORY( Playground )