INCLUDE( MacroInstallFramework )
INCLUDE( MacroExtractArchive )

SET( CRIMILD_INCLUDE_DIRECTORIES
	 ${CRIMILD_SOURCE_DIR}/core/src
)

SET( CRIMILD_LIBRARY_LINK_LIBRARIES
	${CRIMILD_LIBRARY_LINK_LIBRARIES}
	 crimild_core
)

FIND_PACKAGE( Vulkan REQUIRED )

SET( CRIMILD_INCLUDE_DIRECTORIES
  ${CRIMILD_INCLUDE_DIRECTORIES}
		${Vulkan_INCLUDE_DIRS}
)

SET( CRIMILD_LIBRARY_LINK_LIBRARIES
 	  ${CRIMILD_LIBRARY_LINK_LIBRARIES}
		${Vulkan_LIBRARIES}
		#"-framework Metal"
		#"-framework IOSurface"
		#"-framework QuartzCore"
 )

INCLUDE( ModuleBuildLibrary )

ADD_CUSTOM_TARGET(
    crimild_vulkan_shaders
    ALL
	COMMAND sh ${CRIMILD_SOURCE_DIR}/vulkan/src/Rendering/Shaders/build_shaders.sh .
	WORKING_DIRECTORY ${CRIMILD_SOURCE_DIR}/vulkan/src/Rendering/Shaders
	COMMENT "Compiling Crimild::Vulkan built-in shaders"
	VERBATIM
)

ADD_DEPENDENCIES( crimild_vulkan_shaders crimild_core )
