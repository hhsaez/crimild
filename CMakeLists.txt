cmake_minimum_required(VERSION 3.28)

# Register custom scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(crimild_read_version)
include(crimild_trace)
include(crimild_compile_commands)

project(
	Crimild
	VERSION "${CRIMILD_VERSION_MAJOR}.${CRIMILD_VERSION_MINOR}.${CRIMILD_VERSION_PATCH}"
	DESCRIPTION "Crimild Engine"
	LANGUAGES CXX
)

# Define build options here
option(CRIMILD_BUILD_TESTS "Build tests?" ${PROJECT_IS_TOP_LEVEL})
option(CRIMILD_BUILD_EDITOR "Build crimild editor?" ${PROJECT_IS_TOP_LEVEL})
option(CRIMILD_BUILD_PLAYER "Build crimild player?" ${PROJECT_IS_TOP_LEVEL})
option(CRIMILD_BUILD_UNIVERSAL_PLAYER "Build crimild universal player?" ${PROJECT_IS_TOP_LEVEL})
option(CRIMILD_BUILD_EXAMPLES "Build examples?" ${PROJECT_IS_TOP_LEVEL})
option(CRIMILD_BUILD_UNIVERSAL_EXAMPLES "Build examples using universal player?" ${PROJECT_IS_TOP_LEVEL})

# Global tests setup
# Individual directories have to enable their tests too.
if(CRIMILD_BUILD_TESTS)
	include(crimild_configure_tests)
endif()

# Windows-specific compiler flags
if(WIN32)
    # Set runtime library configuration for all targets
    add_compile_options(
        $<$<CONFIG:Debug>:/MDd>
        $<$<CONFIG:Release>:/MD>
        $<$<CONFIG:MinSizeRel>:/MD>
        $<$<CONFIG:RelWithDebInfo>:/MD>
    )

	# Force use of MSVC dynamic runtime libraries
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Add subdirectories
add_subdirectory(libs/foundation)
add_subdirectory(libs/math)
add_subdirectory(libs/coding)
add_subdirectory(core)

# Configure Vulkan
if(CRIMILD_BUILD_EDITOR OR CRIMILD_BUILD_PLAYER)
	add_definitions(-DCRIMILD_ENABLE_VULKAN=1)
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

add_subdirectory(examples)
add_subdirectory(platforms)

# Expose the compilation database at the project root for clangd
# Some editors, like Emacs, need this in order for lsp to work correctly.
crimild_link_compile_commands()
