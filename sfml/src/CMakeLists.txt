SET( CRIMILD_INCLUDE_DIRECTORIES 
	${CRIMILD_SOURCE_DIR}/core/src 
	${CRIMILD_SOURCE_DIR}/third-party/sfml/include 
)

SET( CRIMILD_LIBRARY_LINK_LIBRARIES 
	crimild_core
	sfml-audio
	sfml-graphics
	sfml-network
	sfml-system
	sfml-window 
)

INCLUDE( ModuleBuildLibrary )

