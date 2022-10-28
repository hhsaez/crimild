crimild_trace()

set(
    CRIMILD_LIBRARY_NAME
    crimild_import
)

set( 
    CRIMILD_LIBRARY_SOURCE_FILES 
	Importers/SceneImporter.cpp
	Importers/SceneImporter.hpp
)

add_library(
    ${CRIMILD_LIBRARY_NAME}
    ${CRIMILD_LIBRARY_SOURCE_FILES}
)

target_include_directories( 
	${CRIMILD_LIBRARY_NAME} 
	PUBLIC . 
	PRIVATE ${Assimp_SOURCE_DIR}/include
	PRIVATE ${Assimp_SOURCE_DIR}/code 
)

target_link_directories(
	${CRIMILD_LIBRARY_NAME}
	PRIVATE ${Assimp_BINARY_DIR}
)

target_link_libraries( 
	${CRIMILD_LIBRARY_NAME} 
	PRIVATE crimild_core 
	PRIVATE assimp 
)

target_compile_features( ${CRIMILD_LIBRARY_NAME} PUBLIC cxx_std_20 )
