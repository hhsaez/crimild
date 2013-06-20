CMAKE_MINIMUM_REQUIRED( VERSION 2.8.10 FATAL_ERROR )

# Enable C++11 features
SET( CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -U__STRICT_ANSI__" )

PROJECT( CRIMILD )

# Register custom modules
SET ( CMAKE_MODULE_PATH "${CRIMILD_SOURCE_DIR}/CMakeTools;${CMAKE_MODULE_PATH}" )

INCLUDE( MacroExtractArchive )

# Configuration options
OPTION( CRIMILD_ENABLE_TESTS "Would you like to enable unit tests?" OFF )
IF ( CRIMILD_ENABLE_TESTS )
	ENABLE_TESTING()

	ADD_DEFINITIONS( -DGTEST_LANG_CXX11=1 )
	ADD_DEFINITIONS( -DGTEST_USE_OWN_TR1_TUPLE=1 )

	EXTRACT_ARCHIVE( gmock-1.6.0.zip gmock-1.6.0 ${CRIMILD_SOURCE_DIR}/third-party )

	ADD_SUBDIRECTORY( ${CRIMILD_SOURCE_DIR}/third-party/gmock-1.6.0 "${CMAKE_CURRENT_BINARY_DIR}/third-party/gmock-1.6.0" )
ENDIF ( CRIMILD_ENABLE_TESTS )

# Add sources
ADD_SUBDIRECTORY( core )

OPTION( CRIMILD_ENABLE_GLFW "Would you like to build the crimild-glfw?" OFF )
IF ( CRIMILD_ENABLE_GLFW )
	IF ( NOT EXISTS ${CRIMILD_SOURCE_DIR}/third-party/glfw-2.7.8 )
		EXTRACT_ARCHIVE( glfw-2.7.8.zip glfw-2.7.8 ${CRIMILD_SOURCE_DIR}/third-party )

		IF ( APPLE )
			MESSAGE( "   Building GLFW v2.7.8 for Cocoa" )
			EXECUTE_PROCESS( COMMAND make -s cocoa WORKING_DIRECTORY ${CRIMILD_SOURCE_DIR}/third-party/glfw-2.7.8 )
			EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy 
				${CRIMILD_SOURCE_DIR}/third-party/glfw-2.7.8/lib/cocoa/libglfw.a
				${CRIMILD_SOURCE_DIR}/lib/libglfw.a )
		ENDIF ( APPLE )
	ENDIF ( NOT EXISTS ${CRIMILD_SOURCE_DIR}/third-party/glfw-2.7.8 )

	IF ( NOT EXISTS ${CRIMILD_SOURCE_DIR}/third-party/glew-1.9.0 )
		EXTRACT_ARCHIVE( glew-1.9.0.tgz glew-1.9.0 ${CRIMILD_SOURCE_DIR}/third-party )
	ENDIF ( NOT EXISTS ${CRIMILD_SOURCE_DIR}/third-party/glew-1.9.0 )

	ADD_SUBDIRECTORY( glfw )
ENDIF ( CRIMILD_ENABLE_GLFW )

