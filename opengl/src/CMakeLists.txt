SET( CRIMILD_INCLUDE_DIRECTORIES 
	${CRIMILD_SOURCE_DIR}/core/src 
	${CRIMILD_SOURCE_DIR}/third-party/glfw/include
	${CRIMILD_SOURCE_DIR}/third-party/glew/include )

SET( CRIMILD_LIBRARY_LINK_LIBRARIES 
	crimild_core
	glfw )

IF ( NOT EXISTS "${CRIMILD_SOURCE_DIR}/third-party/glew/src/glew.c" )
   EXECUTE_PROCESS( COMMAND make WORKING_DIRECTORY ${CRIMILD_SOURCE_DIR}/third-party/glew/auto )
ENDIF()

ADD_DEFINITIONS( -DGLEW_STATIC=1 )

# Add GLEW sources as part of the library
SET( CRIMILD_LIBRARY_ADDITIONAL_SOURCE_FILES ${CRIMILD_SOURCE_DIR}/third-party/glew/src/glew.c )

IF ( APPLE )
	SET( CRIMILD_LIBRARY_LINK_LIBRARIES 
		${CRIMILD_LIBRARY_LINK_LIBRARIES} 
		"-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo" )
ELSE ( APPLE )
	SET( CRIMILD_LIBRARY_LINK_LIBRARIES ${CRIMILD_LIBRARY_LINK_LIBRARIES} opengl32 )
ENDIF ( APPLE )

INCLUDE( ModuleBuildLibrary )
