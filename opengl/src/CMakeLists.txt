SET( CRIMILD_INCLUDE_DIRECTORIES 
	${CRIMILD_SOURCE_DIR}/core/src 
	${CRIMILD_SOURCE_DIR}/third-party/glfw/include
	${CRIMILD_SOURCE_DIR}/third-party/glew-1.13.0/include )

SET( CRIMILD_LIBRARY_LINK_LIBRARIES 
	crimild_core
	glfw )

IF ( NOT EXISTS "${CRIMILD_SOURCE_DIR}/third-party/glew-1.13.0/src/glew.c" )
	EXTRACT_ARCHIVE( glew-1.13.0.zip glew-1.13.0 ${CRIMILD_SOURCE_DIR}/third-party )
ENDIF()

ADD_DEFINITIONS( -DGLEW_STATIC=1 )

# Add GLEW sources as part of the library
SET( CRIMILD_LIBRARY_ADDITIONAL_SOURCE_FILES ${CRIMILD_SOURCE_DIR}/third-party/glew-1.13.0/src/glew.c )

IF ( APPLE )
	SET( CRIMILD_LIBRARY_LINK_LIBRARIES 
		${CRIMILD_LIBRARY_LINK_LIBRARIES} 
		"-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo" )
ELSE ( APPLE )
	SET( CRIMILD_LIBRARY_LINK_LIBRARIES ${CRIMILD_LIBRARY_LINK_LIBRARIES} opengl32 )
ENDIF ( APPLE )

INCLUDE( ModuleBuildLibrary )
