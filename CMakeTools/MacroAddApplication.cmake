# This macros setups a crimild-based application 

MACRO( CRIMILD_ADD_APPLICATION appName srcDir )
	SET( CRIMILD_APP_NAME ${appName} )
	SET( CRIMILD_APP_SOURCE_DIRECTORIES ${srcDir} )
	SET( CRIMILD_APP_INCLUDE_DIRECTORIES ${srcDir} )
	INCLUDE( ModuleBuildApp )
ENDMACRO( CRIMILD_ADD_APPLICATION )

