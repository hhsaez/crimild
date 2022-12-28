crimild_trace()

add_library( crimild_import OBJECT )

target_sources( 
    crimild_import

	PUBLIC
	Importers/SceneImporter.hpp

	PRIVATE
	Importers/SceneImporter.cpp
)

target_include_directories( 
	crimild_import
	PUBLIC . 
	PRIVATE ${Assimp_SOURCE_DIR}/include
	PRIVATE ${Assimp_SOURCE_DIR}/code 
)

target_link_directories(
	crimild_import
	PRIVATE ${Assimp_BINARY_DIR}
)

target_link_libraries( 
	crimild_import
	PRIVATE Crimild::Core
	PRIVATE assimp 
)

add_library( Crimild::Import ALIAS crimild_import )

# Keep IDEs folders clean
set_target_properties( assimp PROPERTIES FOLDER extern )
