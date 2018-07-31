# This macro is used to extract and install frameworks on OSX
MACRO( CRIMILD_INSTALL_FRAMEWORK fileName frameworkName )
	SET( FRAMEWORKS_DIR $ENV{HOME}/Library/Frameworks )
	
	IF ( NOT EXISTS ${FRAMEWORKS_DIR} )
	   FILE( MAKE_DIRECTORY ${FRAMEWORKS_DIR} )
	ENDIF ()

	SET( TARGET_FILE_NAME ${FRAMEWORKS_DIR}/${frameworkName}.framework )

	IF ( NOT EXISTS ${TARGET_FILE_NAME} )
	   EXECUTE_PROCESS(
			COMMAND ${CMAKE_COMMAND}
			-E tar -x
			${CRIMILD_SOURCE_DIR}/third-party/${fileName}
			WORKING_DIRECTORY ${FRAMEWORKS_DIR}
	   )
	ENDIF ()
ENDMACRO ()
