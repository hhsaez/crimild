CMAKE_MINIMUM_REQUIRED( VERSION 2.8.10 FATAL_ERROR )

# Enable C++11 features
SET( CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -U__STRICT_ANSI__" )
SET( CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++" )
#set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
#set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++0x -stdlib=libc++ -g -Wall")

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

# Add core sources
ADD_SUBDIRECTORY( core )

# Extract lua sources if needed
EXTRACT_ARCHIVE( lua-5.2.3.tar.gz lua-5.2.3 ${CRIMILD_SOURCE_DIR}/third-party )

# Add scripting sources
ADD_SUBDIRECTORY( scripting )

OPTION( CRIMILD_ENABLE_GLFW "Would you like to build the crimild-glfw extension?" OFF )
IF ( CRIMILD_ENABLE_GLFW )
	IF ( NOT EXISTS ${CRIMILD_SOURCE_DIR}/third-party/glfw-3.0.4 )
		EXTRACT_ARCHIVE( glfw-3.0.4.zip glfw-3.0.4 ${CRIMILD_SOURCE_DIR}/third-party )

	ENDIF ( NOT EXISTS ${CRIMILD_SOURCE_DIR}/third-party/glfw-3.0.4 )

	ADD_SUBDIRECTORY( ${CRIMILD_SOURCE_DIR}/third-party/glfw-3.0.4 )

	IF ( NOT EXISTS ${CRIMILD_SOURCE_DIR}/third-party/glew-1.9.0 )
		EXTRACT_ARCHIVE( glew-1.9.0.tgz glew-1.9.0 ${CRIMILD_SOURCE_DIR}/third-party )
	ENDIF ( NOT EXISTS ${CRIMILD_SOURCE_DIR}/third-party/glew-1.9.0 )

	ADD_SUBDIRECTORY( glfw )
ENDIF ( CRIMILD_ENABLE_GLFW )

OPTION( CRIMILD_ENABLE_AL "Would you like to build the crimild-al extension?" OFF )
IF ( CRIMILD_ENABLE_AL )
	ADD_SUBDIRECTORY( al )
ENDIF ( CRIMILD_ENABLE_AL )

#OPTION( CRIMILD_ENABLE_COLLADA "Would you like to build the crimild-collada extension?" OFF )
#IF ( CRIMILD_ENABLE_COLLADA )
#	ADD_SUBDIRECTORY( collada )
#ENDIF ( CRIMILD_ENABLE_COLLADA )

OPTION( CRIMILD_ENABLE_PHYSICS "Would you like to build the crimild-physics extension?" OFF )
IF ( CRIMILD_ENABLE_PHYSICS )
	ADD_SUBDIRECTORY( physics )
ENDIF ( CRIMILD_ENABLE_PHYSICS )

OPTION( CRIMILD_BUILD_TOOLS "Build tools?" OFF )
IF ( CRIMILD_BUILD_TOOLS )
	ADD_SUBDIRECTORY( tools )
ENDIF ( CRIMILD_BUILD_TOOLS )

