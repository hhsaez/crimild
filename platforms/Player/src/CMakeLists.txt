crimild_trace()

add_library( crimild_player OBJECT )

target_sources(
    crimild_player

    PRIVATE Rendering/PlayerVulkanSurface.cpp
    PRIVATE Rendering/PlayerVulkanSurface.hpp
    PRIVATE Rendering/PlayerWindow.cpp
    PRIVATE Rendering/PlayerWindow.hpp

    PRIVATE Main.cpp
)

target_include_directories(
    crimild_player
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/glfw/include
)

target_link_libraries(
    crimild_player
    PRIVATE Crimild::Foundation
    PRIVATE Crimild::Mathematics
    PRIVATE Crimild::Coding
    PRIVATE Crimild::Core
    PRIVATE Crimild::Vulkan
    PRIVATE glfw
)

add_library( Crimild::Player ALIAS crimild_player )
