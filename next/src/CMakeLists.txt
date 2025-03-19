crimild_trace()

add_library( crimild_next OBJECT )

target_sources(
    crimild_next

    PUBLIC
    Object.hpp
    Node.hpp
    Editable.hpp
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
