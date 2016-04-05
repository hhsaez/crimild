SET( CRIMILD_INCLUDE_DIRECTORIES 
	${CRIMILD_SOURCE_DIR}/core/src 
	${Assimp_SOURCE_DIR}/include
  	${Assimp_SOURCE_DIR}/code )

SET( CRIMILD_LIBRARY_LINK_LIBRARIES 
	crimild_core
	assimp )

SET( CRIMILD_LINK_DIRECTORIES
  	${Assimp_BINARY_DIR} )

INCLUDE( ModuleBuildLibrary )

