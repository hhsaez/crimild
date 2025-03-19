crimild_trace()

add_library( crimild_next OBJECT )

target_sources(
    crimild_next

    PUBLIC
    Crimild_Next.hpp

    PUBLIC
    3D/Geometry3D.hpp
    3D/Group3D.hpp
    3D/Node3D.hpp    
    Object.hpp
    Node.hpp
    Editable.hpp

    PRIVATE
    Crimild_Next.cpp
)

target_include_directories( crimild_next PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

target_compile_features( crimild_next PUBLIC cxx_std_20 )

target_link_libraries(
    crimild_next
    PRIVATE Crimild::Foundation
    PRIVATE Crimild::Mathematics
    PRIVATE Crimild::Coding
)

add_library( Crimild::Next ALIAS crimild_next )
