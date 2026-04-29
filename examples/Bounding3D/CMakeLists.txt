add_executable( Bounding3D )

target_sources( 
    Bounding3D     
    PRIVATE Main.cpp 
)

target_include_directories( 
    Bounding3D
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} 
)

target_link_libraries(
    Bounding3D
    PRIVATE crimild::foundation
    PRIVATE Crimild::Mathematics
    PRIVATE Crimild::Coding
    PRIVATE Crimild::Core
    PRIVATE Crimild::UniversalPlayer
)

set_target_properties(
    Bounding3D
    PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

