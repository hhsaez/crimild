crimild_trace()

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
    Panels/ConsolePanel.cpp
    Panels/ConsolePanel.hpp
    Panels/InspectorPanel.cpp
    Panels/InspectorPanel.hpp
    Panels/MainMenuBar.cpp
    Panels/MainMenuBar.hpp
    Panels/Panel.cpp
    Panels/Panel.hpp
    Panels/PlaybackControlsPanel.cpp
    Panels/PlaybackControlsPanel.hpp
    Panels/ProjectPanel.cpp
    Panels/ProjectPanel.hpp
    Panels/SceneHierarchyPanel.cpp
    Panels/SceneHierarchyPanel.hpp
    Panels/ScenePanel.cpp
    Panels/ScenePanel.hpp
    Panels/SceneRTPanel.cpp
    Panels/SceneRTPanel.hpp
    Panels/SimulationPanel.cpp
    Panels/SimulationPanel.hpp
    Panels/TimelinePanel.cpp
    Panels/TimelinePanel.hpp

    Views/View.hpp
    Views/View.cpp
    Views/Dialogs/Dialog.cpp
    Views/Dialogs/Dialog.hpp
    Views/Dialogs/FileDialog.cpp
    Views/Dialogs/FileDialog.hpp
    Views/Menus/MainMenu/MainMenu.cpp
    Views/Menus/MainMenu/MainMenu.hpp
    Views/Windows/Window.hpp
    Views/Windows/Window.cpp
    Views/Windows/FileSystemWindow.hpp
    Views/Windows/FileSystemWindow.cpp
    Views/Windows/InspectorWindow.hpp
    Views/Windows/InspectorWindow.cpp
    Views/Windows/LogWindow.hpp
    Views/Windows/LogWindow.cpp
    Views/Windows/Scene3DWindow.hpp
    Views/Windows/Scene3DWindow.cpp
    Views/Windows/SceneWindow.hpp
    Views/Windows/SceneWindow.cpp
    Views/Windows/SimulationWindow.hpp
    Views/Windows/SimulationWindow.cpp
    Views/Windows/TimelineWindow.hpp
    Views/Windows/TimelineWindow.cpp
    
    Layout/Layout.hpp
    Layout/Layout.cpp
    Layout/LayoutManager.hpp
    Layout/LayoutManager.cpp
    
    Rendering/STBImageManager.cpp
    Rendering/STBImageManager.hpp

    Rendering/FrameGraph/VulkanRenderSceneDebug.cpp
    Rendering/FrameGraph/VulkanRenderSceneDebug.hpp
    
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
    PRIVATE glfw
)

