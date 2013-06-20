SET( CRIMILD_INCLUDE_DIRECTORIES 
	${CRIMILD_SOURCE_DIR}/core/src 
	${CRIMILD_SOURCE_DIR}/third-party/glfw-2.7.8/include
	${CRIMILD_SOURCE_DIR}/third-party/glew-1.9.0/include )

SET( CRIMILD_LIBRARY_LINK_LIBRARIES 
	core
	glfw )

IF ( APPLE )
	SET( CRIMILD_LIBRARY_SOURCE_FILES ${CRIMILD_SOURCE_DIR}/third-party/glew-1.9.0/src/glew.c )
ENDIF ( APPLE )

INCLUDE( ModuleBuildLibrary )
