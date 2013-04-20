# This macro is used to extract zip file contents
MACRO( EXTRACT_ARCHIVE fileName targetName workingDirectory )
	IF ( NOT EXISTS ${workingDirectory}/${targetName} )
		EXECUTE_PROCESS ( 
			COMMAND ${CMAKE_COMMAND} 
			-E tar -x
			${workingDirectory}/${fileName}
			WORKING_DIRECTORY ${workingDirectory} )
	ENDIF ( NOT EXISTS ${workingDirectory}/${targetName} )
ENDMACRO( EXTRACT_ARCHIVE )
