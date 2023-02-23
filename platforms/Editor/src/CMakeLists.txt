crimild_trace()

find_package( Vulkan REQUIRED )

add_executable( crimild_editor )

target_sources(
    crimild_editor
	
	PRIVATE
    Foundation/GLFWUtils.hpp
    Foundation/ImGuiUtils.hpp
    Editor.hpp
    Editor.cpp
	Main.cpp
    Panels/BehaviorsPanel.cpp
    Panels/BehaviorsPanel.hpp
    Panels/InspectorPanel.cpp
    Panels/InspectorPanel.hpp
    Panels/MainMenuBar.cpp
    Panels/MainMenuBar.hpp
    Panels/PlaybackControlsPanel.cpp
    Panels/PlaybackControlsPanel.hpp
    Panels/ProjectPanel.cpp
    Panels/ProjectPanel.hpp
    Panels/SceneHierarchyPanel.cpp
    Panels/SceneHierarchyPanel.hpp
    Panels/ScenePanel.cpp
    Panels/ScenePanel.hpp
    Panels/SimulationPanel.cpp
    Panels/SimulationPanel.hpp
    Panels/TimelinePanel.cpp
    Panels/TimelinePanel.hpp
    SceneGraph/PrefabNode.cpp
    SceneGraph/PrefabNode.hpp

    PRIVATE
	${PROJECT_SOURCE_DIR}/third-party/imgui/imgui.cpp
	${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_tables.cpp
	${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_draw.cpp
	${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/third-party/imgui//backends/imgui_impl_glfw.cpp
    ${PROJECT_SOURCE_DIR}/third-party/imgui//backends/imgui_impl_vulkan.cpp
	${PROJECT_SOURCE_DIR}/third-party/imguizmo/ImGuizmo.cpp
	${PROJECT_SOURCE_DIR}/third-party/imguizmo/ImCurveEdit.cpp
	${PROJECT_SOURCE_DIR}/third-party/imguizmo/ImSequencer.cpp
	${PROJECT_SOURCE_DIR}/third-party/imguizmo/GraphEditor.cpp
	${PROJECT_SOURCE_DIR}/third-party/ImGuiFileDialog/ImGuiFileDialog.cpp
)

target_include_directories( 
	crimild_editor
	PRIVATE . 
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/glfw/include
    PRIVATE ${PROJECT_SOURCE_DIR}/third-party/glslang
	PRIVATE ${Vulkan_INCLUDE_DIRS} 
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui/examples
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui/backends
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imguizmo
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imguizmo/examples
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/ImGuiFileDialog
)

target_link_libraries( 
	crimild_editor 
	PRIVATE Crimild::Core
    PRIVATE Crimild::Vulkan
	PRIVATE ${Vulkan_LIBRARIES}
    PRIVATE glfw
)

