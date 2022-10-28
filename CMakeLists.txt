cmake_minimum_required( VERSION 3.19 )

# Register custom scripts
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake" )

include( crimild_read_version )
include( crimild_trace )

project(
	Crimild
	VERSION "${CRIMILD_VERSION_MAJOR}.${CRIMILD_VERSION_MINOR}.${CRIMILD_VERSION_PATCH}"
	DESCRIPTION "Crimild Engine"
	LANGUAGES CXX
)

# Check if Crimild is the current project
# This is useful when setting defaults, since some modules are
# only available when Crimild is built as a standalone project.
if( PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME )
	set( CRIMILD_IS_MAIN_PROJECT ON )
else()
	set( CRIMILD_IS_MAIN_PROJECT OFF )
endif()

# Define build options here
option( CRIMILD_BUILD_TESTS "Build tests" ${CRIMILD_IS_MAIN_PROJECT} )
option( CRIMILD_BUILD_IMPORT "Build importers?" ${CRIMILD_IS_MAIN_PROJECT} )
option( CRIMILD_BUILD_STB "Build the STB extension" ${CRIMILD_IS_MAIN_PROJECT} )
option( CRIMILD_BUILD_VULKAN "Build the crimild-vulkan extension?" ${CRIMILD_IS_MAIN_PROJECT} )
option( CRIMILD_BUILD_GLFW "Build the crimild-glfw extension?" ${CRIMILD_IS_MAIN_PROJECT} )
option( CRIMILD_BUILD_IMGUI "Build the crimild-imgui extension?" ${CRIMILD_IS_MAIN_PROJECT} )
option( CRIMILD_BUILD_EXAMPLES "Build examples?" ${CRIMILD_IS_MAIN_PROJECT} )

# Global tests setup
# Individual directories have to enable their tests too.
if( CRIMILD_BUILD_TESTS )
	include( crimild_configure_tests )
endif ()

# Add subdirectories
add_subdirectory( core )

if( CRIMILD_BUILD_IMPORT )
	add_definitions( -DCRIMILD_ENABLE_IMPORT=1 )

	set( ASSIMP_BUILD_STATIC_LIB ON CACHE BOOL "Build Static libs" )
	set( BUILD_SHARED_LIBS OFF CACHE BOOL "Build Shared Libs" )
	set( ASSIMP_BUILD_TESTS OFF CACHE BOOL "Build Assimp tests" )
	set( ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Build Assimp tools" )
	add_subdirectory( ${PROJECT_SOURCE_DIR}/third-party/assimp )

	add_subdirectory( import )
endif()

if( CRIMILD_BUILD_STB )
	add_subdirectory( stb )
endif()

if( CRIMILD_BUILD_VULKAN )
	add_definitions( -DCRIMILD_ENABLE_VULKAN=1 )
	
	set( SKIP_GLSLANG_INSTALL ON CACHE BOOL "Skip installation" )
	set( ENABLE_CTEST OFF CACHE BOOL "Enables testing" )
	set( BUILD_EXTERNAL OFF CACHE BOOL "Build external dependencies in /External" )
	set( ENABLE_SPVREMAPPER OFF CACHE BOOL "Enables building of SPVRemapper" )
	set( ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "Builds glslangValidator and spirv-remap" )
	add_subdirectory( ${Crimild_SOURCE_DIR}/third-party/glslang )
	
	add_subdirectory(vulkan)
endif()

if( CRIMILD_BUILD_IMGUI )
	add_definitions( -DCRIMILD_ENABLE_IMGUI=1 )
	add_subdirectory( imgui )
endif()

if( CRIMILD_BUILD_GLFW )
	add_definitions( -DCRIMILD_ENABLE_GLFW=1 )

	set( GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" )
	set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" )
	set( GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" )
	set( ENABLE_CTEST OFF CACHE BOOL "Enables testing" )
	add_subdirectory( ${Crimild_SOURCE_DIR}/third-party/glfw )

	add_subdirectory( glfw )
endif()

if( CRIMILD_BUILD_EXAMPLES )
	add_subdirectory( examples )
endif()
