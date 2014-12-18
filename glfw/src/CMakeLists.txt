SET( CRIMILD_INCLUDE_DIRECTORIES 
	${CRIMILD_SOURCE_DIR}/core/src 
	${CRIMILD_SOURCE_DIR}/scripting/src 
	${CRIMILD_SOURCE_DIR}/physics/src 
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src
	${CRIMILD_SOURCE_DIR}/third-party/bullet-2.82-r2704/src
	${CRIMILD_SOURCE_DIR}/third-party/glfw-3.0.4/include
	${CRIMILD_SOURCE_DIR}/third-party/glew-1.9.0/include )

SET( CRIMILD_LIBRARY_LINK_LIBRARIES 
	crimild_core
	crimild_scripting
	crimild_physics
	glfw )

# Add GLEW sources as part of the library
SET( CRIMILD_LIBRARY_ADDITIONAL_SOURCE_FILES ${CRIMILD_SOURCE_DIR}/third-party/glew-1.9.0/src/glew.c )

INCLUDE( ModuleBuildLibrary )
