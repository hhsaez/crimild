crimild_trace()

set(
    CRIMILD_LIBRARY_NAME
    crimild_stb
)

set( 
    CRIMILD_LIBRARY_SOURCE_FILES 
	Rendering/STBImageManager.cpp
	Rendering/STBImageManager.hpp
	Crimild_STB.cpp
	Crimild_STB.hpp
)

add_library(
    ${CRIMILD_LIBRARY_NAME}
    ${CRIMILD_LIBRARY_SOURCE_FILES}
)

target_include_directories( 
	${CRIMILD_LIBRARY_NAME} 
	PUBLIC . 
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/stb
)

target_link_libraries( 
	${CRIMILD_LIBRARY_NAME} 
	PRIVATE crimild_core 
)

target_compile_features( ${CRIMILD_LIBRARY_NAME} PUBLIC cxx_std_20 )
