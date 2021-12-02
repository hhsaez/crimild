SET( CRIMILD_APP_NAME TextureFilters )
SET( CRIMILD_APP_SOURCE_DIRECTORIES "." )
SET( CRIMILD_APP_INCLUDE_DIRECTORIES "." )

INCLUDE( ModuleBuildApp )

#TODO: Add this to ModuleBuildApp
#TODO 2: Support Xcode ENV variables in Schemas
ADD_CUSTOM_COMMAND(
	TARGET ${CRIMILD_APP_NAME}
	PRE_BUILD
	COMMAND sh
	ARGS ${CRIMILD_SOURCE_DIR}/vulkan/src/Rendering/Shaders/build_shaders.sh .
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders
	COMMENT "Building shaders for ${CRIMILD_APP_NAME}"
	VERBATIM
)
