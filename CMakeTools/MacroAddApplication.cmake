# This macros setups a crimild-based application 

MACRO( CRIMILD_ADD_APPLICATION appName srcDir )
	SET( CRIMILD_APP_NAME ${appName} )
	SET( CRIMILD_APP_SOURCE_DIR ${srcDir} )
	INCLUDE( ModuleBuildApp )
ENDMACRO( CRIMILD_ADD_APPLICATION )

