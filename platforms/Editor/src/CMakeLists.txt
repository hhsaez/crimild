crimild_trace()

add_executable( crimild_editor )

target_sources(
    crimild_editor

    PRIVATE Foundation/GLFWUtils.hpp
    PRIVATE Foundation/ImGuiUtils.cpp
    PRIVATE Foundation/ImGuiUtils.hpp
    PRIVATE Foundation/STBUtils.cpp
    PRIVATE Foundation/STBUtils.hpp

    PRIVATE Importers/GLTFImporter.cpp
    PRIVATE Importers/GLTFImporter.hpp

    PRIVATE Views/View.hpp
    PRIVATE Views/View.cpp
    PRIVATE Views/Dialogs/Dialog.cpp
    PRIVATE Views/Dialogs/Dialog.hpp
    PRIVATE Views/Dialogs/FileDialog.cpp
    PRIVATE Views/Dialogs/FileDialog.hpp

    PRIVATE Views/Menus/MainMenu/MainMenu.cpp
    PRIVATE Views/Menus/MainMenu/MainMenu.hpp
    PRIVATE Views/Menus/NewEntityMenu.hpp

    PRIVATE Views/Modals/AboutModal.cpp
    PRIVATE Views/Modals/AboutModal.hpp
    PRIVATE Views/Modals/AlertModal.cpp
    PRIVATE Views/Modals/AlertModal.hpp
    PRIVATE Views/Modals/Modal.cpp
    PRIVATE Views/Modals/Modal.hpp

    PRIVATE Views/Panels/Scene3D/Scene3DPanel.cpp
    PRIVATE Views/Panels/Scene3D/Scene3DPanel.hpp

    PRIVATE Views/Specials/ImGuiDemoWindow.hpp
    PRIVATE Views/Specials/ImGuiDemoWindow.cpp
    PRIVATE Views/Windows/BehaviorsWindow.cpp
    PRIVATE Views/Windows/BehaviorsWindow.hpp
    PRIVATE Views/Windows/FileSystemWindow.hpp
    PRIVATE Views/Windows/FileSystemWindow.cpp

    PRIVATE Views/Windows/GraphEditor/Editables/Renderers/SceneGraph/GroupRenderer.cpp
    PRIVATE Views/Windows/GraphEditor/Editables/Renderers/SceneGraph/GroupRenderer.hpp
    PRIVATE Views/Windows/GraphEditor/Editables/Renderers/SceneGraph/NodeRenderer.cpp
    PRIVATE Views/Windows/GraphEditor/Editables/Renderers/SceneGraph/NodeRenderer.hpp

    PRIVATE Views/Windows/GraphEditor/Editables/Editable.cpp
    PRIVATE Views/Windows/GraphEditor/Editables/Editable.hpp

    PRIVATE Views/Windows/GraphEditor/Builders.cpp
    PRIVATE Views/Windows/GraphEditor/Builders.hpp
    PRIVATE Views/Windows/GraphEditor/GraphEditorContext.hpp
    PRIVATE Views/Windows/GraphEditor/GraphEditorTypes.hpp
    PRIVATE Views/Windows/GraphEditor/GraphEditorWindow.cpp
    PRIVATE Views/Windows/GraphEditor/GraphEditorWindow.hpp

    PRIVATE Views/Windows/InspectorWindow.hpp
    PRIVATE Views/Windows/InspectorWindow.cpp
    PRIVATE Views/Windows/LogWindow.hpp
    PRIVATE Views/Windows/LogWindow.cpp
    PRIVATE Views/Windows/PlaybackControlsWindow.cpp
    PRIVATE Views/Windows/PlaybackCOntrolsWindow.hpp
    PRIVATE Views/Windows/Scene3DWindow.hpp
    PRIVATE Views/Windows/Scene3DWindow.cpp
    PRIVATE Views/Windows/SceneRTWindow.cpp
    PRIVATE Views/Windows/SceneRTWindow.hpp
    PRIVATE Views/Windows/SceneWindow.hpp
    PRIVATE Views/Windows/SceneWindow.cpp
    PRIVATE Views/Windows/SimulationWindow.hpp
    PRIVATE Views/Windows/SimulationWindow.cpp
    PRIVATE Views/Windows/TimelineWindow.hpp
    PRIVATE Views/Windows/TimelineWindow.cpp
    PRIVATE Views/Windows/Window.hpp
    PRIVATE Views/Windows/Window.cpp

    PRIVATE Views/Workspaces/Workspace.hpp
    PRIVATE Views/Workspaces/Workspace.cpp
    PRIVATE Views/Workspaces/AssemblyWorkspace.hpp
    PRIVATE Views/Workspaces/AssemblyWorkspace.cpp
    PRIVATE Views/Workspaces/WorkspaceManager.hpp
    PRIVATE Views/Workspaces/WorkspaceManager.cpp

    PRIVATE Layout/Layout.hpp
    PRIVATE Layout/Layout.cpp
    PRIVATE Layout/LayoutManager.hpp
    PRIVATE Layout/LayoutManager.cpp

    PRIVATE Rendering/GLFWVulkanSurface.cpp
    PRIVATE Rendering/GLFWVUlkanSurface.hpp
    PRIVATE Rendering/STBImageManager.cpp
    PRIVATE Rendering/STBImageManager.hpp

    PRIVATE Rendering/FrameGraph/VulkanRenderSceneDebug.cpp
    PRIVATE Rendering/FrameGraph/VulkanRenderSceneDebug.hpp

    PRIVATE SceneGraph/PrefabNode.cpp
    PRIVATE SceneGraph/PrefabNode.hpp
    PRIVATE Simulation/Editor.hpp

    PRIVATE Simulation/Editor.cpp
    PRIVATE Simulation/Project.hpp
    PRIVATE Simulation/Project.cpp

    PRIVATE Main.cpp

	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui/imgui.cpp
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_tables.cpp
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_demo.cpp
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_draw.cpp
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_widgets.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui//backends/imgui_impl_glfw.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui//backends/imgui_impl_vulkan.cpp

	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imguizmo/ImGuizmo.cpp
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imguizmo/ImCurveEdit.cpp
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imguizmo/ImSequencer.cpp
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imguizmo/GraphEditor.cpp

	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/ImGuiFileDialog/ImGuiFileDialog.cpp

    PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui-node-editor/crude_json.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui-node-editor/imgui_canvas.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui-node-editor/imgui_node_editor.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui-node-editor/imgui_node_editor_api.cpp
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
    PRIVATE crimild::foundation
    PRIVATE Crimild::Mathematics
    PRIVATE crimild::coding
	PRIVATE crimild::core
    PRIVATE Crimild::Vulkan
    PRIVATE glfw
)
