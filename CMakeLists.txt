cmake_minimum_required(VERSION 3.16)

# Register custom scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(crimild_read_version)
include(crimild_trace)

project(
	Crimild
	VERSION "${CRIMILD_VERSION_MAJOR}.${CRIMILD_VERSION_MINOR}.${CRIMILD_VERSION_PATCH}"
	DESCRIPTION "Crimild Engine"
	LANGUAGES CXX
)

# Check if Crimild is the current project
# This is useful when setting defaults, since some modules are
# only available when Crimild is built as a standalone project.
if(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
	set(CRIMILD_IS_MAIN_PROJECT ON)
else()
	set(CRIMILD_IS_MAIN_PROJECT OFF)
endif()

# Define build options here
option(CRIMILD_BUILD_TESTS "Build tests?" ${CRIMILD_IS_MAIN_PROJECT})
option(CRIMILD_BUILD_EDITOR "Build crimild editor?" ${CRIMILD_IS_MAIN_PROJECT})
option(CRIMILD_BUILD_PLAYER "Build crimild player?" ${CRIMILD_IS_MAIN_PROJECT})
option(CRIMILD_BUILD_EXAMPLES "Build examples?" ${CRIMILD_IS_MAIN_PROJECT})

# Global tests setup
# Individual directories have to enable their tests too.
if(CRIMILD_BUILD_TESTS)
	include(crimild_configure_tests)
endif()

# Add subdirectories
add_subdirectory(foundation)
add_subdirectory(mathematics)
add_subdirectory(coding)
add_subdirectory(core)

# Configure Vulkan
if(CRIMILD_BUILD_EDITOR OR CRIMILD_BUILD_PLAYER)
	add_definitions(-DCRIMILD_ENABLE_VULKAN=1)

	if(NOT APPLE)
		set(SKIP_GLSLANG_INSTALL ON CACHE BOOL "Skip installation")
		set(ENABLE_CTEST OFF CACHE BOOL "Enables testing")
		set(BUILD_EXTERNAL OFF CACHE BOOL "Build external dependencies in /External")
		set(ENABLE_SPVREMAPPER OFF CACHE BOOL "Enables building of SPVRemapper")
		set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "Builds glslangValidator and spirv-remap")
		set(ALLOW_EXTERNAL_SPIRV_TOOLS ON CACHE BOOL "Allows to build against installed SPIRV-Tools-opt. This is unsupported if the commit isn't the one in known_good.json")
		set(ENABLE_OPT OFF CACHE BOOL "Enables building of SPIRV-Tools-opt")
		add_subdirectory(${Crimild_SOURCE_DIR}/third-party/glslang)
	endif()

	add_subdirectory(vulkan)
endif()

# Configure GLFW
if(CRIMILD_BUILD_EDITOR OR CRIMILD_BUILD_PLAYER)
	set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
	set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
	set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
	set(ENABLE_CTEST OFF CACHE BOOL "Enables testing")
	add_subdirectory(${Crimild_SOURCE_DIR}/third-party/glfw)
endif()

# Configure TinyGLTF
if(CRIMILD_BUILD_EDITOR OR CRIMILD_BUILD_PLAYER)
	set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE)
	set(TINYGLTF_INSTALL OFF CACHE INTERNAL "" FORCE)
	add_subdirectory(${PROJECT_SOURCE_DIR}/third-party/tinygltf)
endif()

if(CRIMILD_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

add_subdirectory(platforms)
