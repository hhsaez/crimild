crimild_trace()

find_package( Vulkan REQUIRED )

add_executable( crimild_editor )

target_sources(
    crimild_editor
	
	PRIVATE
    Foundation/GLFWUtils.hpp
    Foundation/ImGuiUtils.cpp
    Foundation/ImGuiUtils.hpp
    Foundation/STBUtils.cpp
    Foundation/STBUtils.hpp
    Importers/GLTFImporter.cpp
    Importers/GLTFImporter.hpp
    Panels/BehaviorsPanel.cpp
    Panels/BehaviorsPanel.hpp
    Panels/InspectorPanel.cpp
    Panels/InspectorPanel.hpp
    Panels/MainMenuBar.cpp
    Panels/MainMenuBar.hpp
    Panels/Panel.hpp
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
    Rendering/STBImageManager.cpp
    Rendering/STBImageManager.hpp
    SceneGraph/PrefabNode.cpp
    SceneGraph/PrefabNode.hpp
    Simulation/Editor.hpp
    Simulation/Editor.cpp
    Simulation/Project.hpp
    Simulation/Project.cpp
	Main.cpp

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
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/stb
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/tinygltf
)

target_link_libraries( 
	crimild_editor 
	PRIVATE Crimild::Core
    PRIVATE Crimild::Vulkan
	PRIVATE ${Vulkan_LIBRARIES}
    PRIVATE glfw
)

