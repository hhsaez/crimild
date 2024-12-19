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

    Views/View.hpp
    Views/View.cpp
    Views/Dialogs/Dialog.cpp
    Views/Dialogs/Dialog.hpp
    Views/Dialogs/FileDialog.cpp
    Views/Dialogs/FileDialog.hpp
    Views/Menus/MainMenu/MainMenu.cpp
    Views/Menus/MainMenu/MainMenu.hpp
    Views/Modals/AboutModal.cpp
    Views/Modals/AboutModal.hpp
    Views/Modals/AlertModal.cpp
    Views/Modals/AlertModal.hpp
    Views/Modals/Modal.cpp
    Views/Modals/Modal.hpp
    Views/Specials/ImGuiDemoWindow.hpp
    Views/Specials/ImGuiDemoWindow.cpp
    Views/Windows/BehaviorsWindow.cpp
    Views/Windows/BehaviorsWindow.hpp
    Views/Windows/FileSystemWindow.hpp
    Views/Windows/FileSystemWindow.cpp
    Views/Windows/GraphEditor/Builders.cpp
    Views/Windows/GraphEditor/Builders.hpp
    Views/Windows/GraphEditor/GraphEditorWindow.cpp
    Views/Windows/GraphEditor/GraphEditorWindow.hpp
    Views/Windows/InspectorWindow.hpp
    Views/Windows/InspectorWindow.cpp
    Views/Windows/LogWindow.hpp
    Views/Windows/LogWindow.cpp
    Views/Windows/PlaybackControlsWindow.cpp
    Views/Windows/PlaybackCOntrolsWindow.hpp
    Views/Windows/Scene3DWindow.hpp
    Views/Windows/Scene3DWindow.cpp
    Views/Windows/SceneRTWindow.cpp
    Views/Windows/SceneRTWindow.hpp
    Views/Windows/SceneWindow.hpp
    Views/Windows/SceneWindow.cpp
    Views/Windows/SimulationWindow.hpp
    Views/Windows/SimulationWindow.cpp
    Views/Windows/TimelineWindow.hpp
    Views/Windows/TimelineWindow.cpp
    Views/Windows/Window.hpp
    Views/Windows/Window.cpp

    Layout/Layout.hpp
    Layout/Layout.cpp
    Layout/LayoutManager.hpp
    Layout/LayoutManager.cpp

    Rendering/GLFWVulkanSurface.cpp
    Rendering/GLFWVUlkanSurface.hpp
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

    ${PROJECT_SOURCE_DIR}/third-party/imgui-node-editor/crude_json.cpp
    ${PROJECT_SOURCE_DIR}/third-party/imgui-node-editor/imgui_canvas.cpp
    ${PROJECT_SOURCE_DIR}/third-party/imgui-node-editor/imgui_node_editor.cpp
    ${PROJECT_SOURCE_DIR}/third-party/imgui-node-editor/imgui_node_editor_api.cpp
)

target_include_directories(
	crimild_editor
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/glfw/include
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui/examples
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui/backends
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imguizmo
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imguizmo/examples
    PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui-node-editor
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/ImGuiFileDialog
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/stb
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/tinygltf
)

target_link_libraries(
	crimild_editor
    PRIVATE Crimild::Foundation
    PRIVATE Crimild::Mathematics
    PRIVATE Crimild::Coding
	PRIVATE Crimild::Core
    PRIVATE Crimild::Vulkan
    PRIVATE glfw
)
