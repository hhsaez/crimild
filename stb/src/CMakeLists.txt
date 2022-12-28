crimild_trace()

add_library( crimild_stb OBJECT )
target_sources( 
    crimild_stb

	PUBLIC
	Crimild_STB.hpp
	Rendering/STBImageManager.hpp

	PRIVATE
	Crimild_STB.cpp
	Rendering/STBImageManager.cpp
)

target_include_directories( 
	crimild_stb
	PUBLIC . 
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/stb
)

target_link_libraries( 
	crimild_stb
	PRIVATE Crimild::Core
)

add_library( Crimild::STB ALIAS crimild_stb )

