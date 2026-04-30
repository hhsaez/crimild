add_executable( Triangle3D )

target_sources( 
    Triangle3D     
    PRIVATE Main.cpp 
)

target_include_directories( 
    Triangle3D
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} 
)

target_link_libraries(
    Triangle3D
    PRIVATE crimild::foundation
    PRIVATE Crimild::Mathematics
    PRIVATE Crimild::Coding
    PRIVATE Crimild::Core
    PRIVATE Crimild::UniversalPlayer
)

set_target_properties(
    Triangle3D
    PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

