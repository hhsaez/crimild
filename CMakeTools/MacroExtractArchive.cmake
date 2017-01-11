# This macro is used to extract zip file contents
MACRO( EXTRACT_ARCHIVE fileName targetName workingDirectory )
	IF ( NOT EXISTS ${workingDirectory}/${targetName} )
		EXECUTE_PROCESS ( 
			COMMAND ${CMAKE_COMMAND} 
			-E tar -x
			${workingDirectory}/${fileName}
			WORKING_DIRECTORY ${workingDirectory} )

		string(REGEX REPLACE "\\.[^.]*$" "" outputFileName ${fileName})


		#GET_FILENAME_COMPONENT( outputFileName ${fileName} NAME_WE )

		MESSAGE( "lalalal " ${fileName} ${outputFileName} ${targetName} )

		IF ( NOT ${outputFileName} STREQUAL ${targetName} )
			FILE( RENAME ${workingDirectory}/${outputFileName} ${workingDirectory}/${targetName} )
		ENDIF ( NOT ${outputFileName} STREQUAL ${targetName} )

	ENDIF ( NOT EXISTS ${workingDirectory}/${targetName} )
ENDMACRO( EXTRACT_ARCHIVE )
