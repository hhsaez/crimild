CMAKE_MINIMUM_REQUIRED( VERSION 3.1.0 FATAL_ERROR )

PROJECT( CRIMILD )

# Register custom modules
SET ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CRIMILD_SOURCE_DIR}/CMakeTools" )

INCLUDE( MacroExtractArchive )
INCLUDE( MacroConfigureSystem )

ADD_DEFINITIONS( -DCRIMILD_VERSION_MAJOR=5 )
ADD_DEFINITIONS( -DCRIMILD_VERSION_MINOR=0 )
ADD_DEFINITIONS( -DCRIMILD_VERSION_PATCH=0 )

# Configuration options
OPTION( CRIMILD_ENABLE_TESTS "Would you like to enable unit tests?" ON )
OPTION( CRIMILD_ENABLE_IMPORT "Build importers?" ON )
OPTION( CRIMILD_ENABLE_STB "Would you like to build the crimild-stb extension" ON )
OPTION( CRIMILD_ENABLE_SCRIPTING "Would you like to build the crimild-scripting extension?" ON )
OPTION( CRIMILD_ENABLE_OPENAL "Would you like to build the crimild-openal extension" ON )
OPTION( CRIMILD_ENABLE_GLFW "Would you like to build the crimild-glfw extension?" ON )
OPTION( CRIMILD_ENABLE_VULKAN "Would you like to build the crimild-vulkan extension?" ON )
OPTION( CRIMILD_ENABLE_IMGUI "Would you like to build the crimild-imgui extension?" ON )
OPTION( CRIMILD_SCRIPTING_LOG_VERBOSE "Enable verbose logs for scripting system" OFF )
OPTION( CRIMILD_BUILD_TOOLS "Build tools?" ON )
OPTION( CRIMILD_BUNDLE_APPS "Enable bundles for apps" OFF )
OPTION( CRIMILD_BUILD_EXAMPLES "Would you like to build examples?" ON )

# DEPRECATED
OPTION( CRIMILD_ENABLE_PHYSICS "(DEPRECATED) Would you like to build the crimild-physics extension?" OFF )

IF ( ${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten" )
   SET( CRIMILD_ENABLE_EMSCRIPTEN ON )
ELSE ()
   SET( CRIMILD_ENABLE_EMSCRIPTEN OFF )
ENDIF ()

IF ( CRIMILD_ENABLE_TESTS )
	ENABLE_TESTING()

	# For Windows: Prevent overriding the parent project's compiler/linker settings
	SET( gtest_force_shared_crt ON CACHE BOOL "" FORCE )

	ADD_SUBDIRECTORY( ${CRIMILD_SOURCE_DIR}/third-party/googletest )
ENDIF ( CRIMILD_ENABLE_TESTS )

# Add core sources
ADD_SUBDIRECTORY( core )
#ADD_SUBDIRECTORY( raytracing )

IF ( CRIMILD_ENABLE_STB )
   ADD_SUBDIRECTORY( stb )
ENDIF ()

IF ( CRIMILD_ENABLE_IMGUI )
	ADD_DEFINITIONS( -DCRIMILD_ENABLE_IMGUI=1 )
   	ADD_SUBDIRECTORY( imgui )
ENDIF ()

IF ( CRIMILD_ENABLE_IMPORT )
	ADD_DEFINITIONS( -DCRIMILD_ENABLE_IMPORT=1 )

	SET( ASSIMP_BUILD_STATIC_LIB ON CACHE BOOL "Build Static libs" )
	SET( BUILD_SHARED_LIBS OFF CACHE BOOL "Build Shared Libs" )
	SET( ASSIMP_BUILD_TESTS OFF CACHE BOOL "Build Assimp tests" )
	SET( ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Build Assimp tools" )

	ADD_SUBDIRECTORY( ${CRIMILD_SOURCE_DIR}/third-party/assimp )
	ADD_SUBDIRECTORY( import )
ENDIF ( CRIMILD_ENABLE_IMPORT )

IF ( CRIMILD_ENABLE_PHYSICS )
	ADD_DEFINITIONS( -DCRIMILD_ENABLE_PHYSICS=1 )
	ADD_SUBDIRECTORY( physics )
ENDIF ( CRIMILD_ENABLE_PHYSICS )

# Add scripting sources
iF ( CRIMILD_ENABLE_SCRIPTING )
	ADD_DEFINITIONS( -DCRIMILD_ENABLE_SCRIPTING=1 )

	# Extract lua sources if needed
	EXTRACT_ARCHIVE( lua-5.3.4.tar.gz lua ${CRIMILD_SOURCE_DIR}/third-party )

	IF ( CRIMILD_SCRIPTING_LOG_VERBOSE )
		ADD_DEFINITIONS( -DCRIMILD_SCRIPTING_LOG_VERBOSE=1 )
	ENDIF ()
	ADD_SUBDIRECTORY( scripting )
ENDIF ()

IF ( CRIMILD_ENABLE_OPENAL )
	IF ( APPLE )
		ADD_DEFINITIONS( -DCRIMILD_ENABLE_OPENAL=1 )
		ADD_SUBDIRECTORY( openal )
	ELSE()
		MESSAGE( "OpenAL extensions are not supported in this system" )
	ENDIF()
ENDIF ()

IF ( CRIMILD_ENABLE_VULKAN )
   SET( SKIP_GLSLANG_INSTALL ON CACHE BOOL "Skip installation" )
   SET( ENABLE_CTEST OFF CACHE BOOL "Enables testing" )
   SET( BUILD_EXTERNAL OFF CACHE BOOL "Build external dependencies in /External" )
   SET( ENABLE_SPVREMAPPER OFF CACHE BOOL "Enables building of SPVRemapper" )
   SET( ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "Builds glslangValidator and spirv-remap" )
   ADD_SUBDIRECTORY( ${CRIMILD_SOURCE_DIR}/third-party/glslang )

   ADD_DEFINITIONS( -DCRIMILD_ENABLE_VULKAN=1 )
   ADD_SUBDIRECTORY( vulkan )
ENDIF ()

IF ( CRIMILD_ENABLE_GLFW )
	ADD_DEFINITIONS( -DCRIMILD_ENABLE_GLFW=1 )

	IF ( NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten" )
	   	SET( GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" )
	   	SET( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" )
		SET( GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" )
        SET( ENABLE_CTEST OFF CACHE BOOL "Enables testing" )


		ADD_SUBDIRECTORY( ${CRIMILD_SOURCE_DIR}/third-party/glfw )
	ELSE ()
   		 SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3" )
	ENDIF ()

	ADD_SUBDIRECTORY( glfw )

ENDIF ( CRIMILD_ENABLE_GLFW )

IF ( CRIMILD_BUILD_TOOLS )
	IF ( APPLE )
		ADD_SUBDIRECTORY( tools )
	ELSE()
		MESSAGE( "Tools are not supported in this system" )
	ENDIF()
ENDIF ( CRIMILD_BUILD_TOOLS )

IF ( CRIMILD_BUILD_EXAMPLES )
	ADD_SUBDIRECTORY( examples )
ENDIF()
