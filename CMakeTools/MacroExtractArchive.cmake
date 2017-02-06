# This macro is used to extract zip file contents
MACRO( EXTRACT_ARCHIVE fileName targetName workingDirectory )
	IF ( NOT EXISTS ${workingDirectory}/${targetName} )
		MESSAGE( "Extracting " ${fileName} )
		
		EXECUTE_PROCESS ( 
			COMMAND ${CMAKE_COMMAND} 
			-E tar -x
			${workingDirectory}/${fileName}
			WORKING_DIRECTORY ${workingDirectory} )

		STRING( FIND ${fileName} ".tar.gz" IS_TAR_GZ )

		IF ( ${IS_TAR_GZ} GREATER -1 ) 
			STRING( REPLACE ".tar.gz" "" outputFileName ${fileName})
		ELSE ()
			STRING( REPLACE ".zip" "" outputFileName ${fileName})
		ENDIF ()

		IF ( NOT ${outputFileName} STREQUAL ${targetName} )
			MESSAGE( "Renaming " ${outputFileName} " to " ${targetName} )
			FILE( RENAME ${workingDirectory}/${outputFileName} ${workingDirectory}/${targetName} )
		ENDIF ( NOT ${outputFileName} STREQUAL ${targetName} )

	ENDIF ( NOT EXISTS ${workingDirectory}/${targetName} )
ENDMACRO( EXTRACT_ARCHIVE )
