INCLUDE( MacroInstallFramework )
INCLUDE( MacroExtractArchive )

SET( CRIMILD_INCLUDE_DIRECTORIES
	 ${CRIMILD_SOURCE_DIR}/core/src
)

SET( CRIMILD_LIBRARY_LINK_LIBRARIES
	${CRIMILD_LIBRARY_LINK_LIBRARIES}
	 crimild_core
)

IF ( APPLE )
   EXTRACT_ARCHIVE( vulkansdk-macos.zip vulkansdk ${CRIMILD_SOURCE_DIR}/third-party )

   SET(
		CRIMILD_INCLUDE_DIRECTORIES
		${CRIMILD_INCLUDE_DIRECTORIES}
		${CRIMILD_SOURCE_DIR}/third-party/vulkansdk/include
   )
ENDIF ()

INCLUDE( ModuleBuildLibrary )

