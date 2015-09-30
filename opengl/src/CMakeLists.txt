SET( CRIMILD_INCLUDE_DIRECTORIES 
	${CRIMILD_SOURCE_DIR}/core/src 
	${CRIMILD_SOURCE_DIR}/third-party/glfw/include
	${CRIMILD_SOURCE_DIR}/third-party/glew/include )

SET( CRIMILD_LIBRARY_LINK_LIBRARIES 
	crimild_core
	glfw )

# Add GLEW sources as part of the library
SET( CRIMILD_LIBRARY_ADDITIONAL_SOURCE_FILES ${CRIMILD_SOURCE_DIR}/third-party/glew/src/glew.c )

INCLUDE( ModuleBuildLibrary )
