function( CRIMILD_READ_VERSION CRIMILD_VERSION_NAME_ARG )
	set( VERSION_REGEX "#define ${CRIMILD_VERSION_NAME_ARG}[ \t]+(.+)" )
	file( 
		STRINGS 
		"${CMAKE_CURRENT_SOURCE_DIR}/core/src/Foundation/Version.hpp" 
		VERSION_STRING 
		REGEX 
		${VERSION_REGEX} 
	)
	string( REGEX REPLACE ${VERSION_REGEX} "\\1" VERSION_STRING "${VERSION_STRING}" )
	set( ${CRIMILD_VERSION_NAME_ARG} ${VERSION_STRING} PARENT_SCOPE )
endfunction()

# Read Crimild's version from Version header file
crimild_read_version( CRIMILD_VERSION_MAJOR )
crimild_read_version( CRIMILD_VERSION_MINOR )
crimild_read_version( CRIMILD_VERSION_PATCH )
