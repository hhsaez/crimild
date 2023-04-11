crimild_trace()
#crimild_add_example( Triangle Main.cpp )

add_executable( Triangle )
target_sources( Triangle PRIVATE Main.cpp )
target_include_directories( Triangle PRIVATE . )
target_link_libraries(
    Triangle
    
    PRIVATE
    PRIVATE Crimild::Core
    PRIVATE Crimild::OpenGL::Player
)
set_target_properties( Triangle PROPERTIES FOLDER examples )
