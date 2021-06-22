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

     # Make sure to include GLSLang headers before Vulkan's so there is no conflict and
     # the app will run correctly. Otherwise, you may seem errors when compiling shaders
     # due to invalid resource settings (like when using gl_VertexIdentifier in arrays)
     ${CRIMILD_SOURCE_DIR}/third-party/glslang
     ${Vulkan_INCLUDE_DIRS}
)

SET( CRIMILD_LIBRARY_LINK_LIBRARIES
 	  ${CRIMILD_LIBRARY_LINK_LIBRARIES}
      ${Vulkan_LIBRARIES}
      glslang
      SPIRV
 )

INCLUDE( ModuleBuildLibrary )
