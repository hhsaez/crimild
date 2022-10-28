crimild_trace()

set(
    CRIMILD_LIBRARY_NAME
    crimild_glfw
)

set( 
    CRIMILD_LIBRARY_SOURCE_FILES 
	Coding/JSONDecoder.cpp
	Coding/JSONDecoder.hpp
	Coding/JSONEncoder.cpp
	Coding/JSONEncoder.hpp
	Crimild_GLFW.cpp
	Crimild_GLFW.hpp
	Editor/EditorLayer.cpp
	Editor/EditorLayer.hpp
	Editor/EditorState.cpp
	Editor/EditorState.hpp
	Editor/EditorUtils.cpp
	Editor/EditorUtils.hpp
	Editor/Menus/editMenu.cpp
	Editor/Menus/editMenu.hpp
	Editor/Menus/fileMenu.cpp
	Editor/Menus/fileMenu.hpp
	Editor/Menus/helpMenu.cpp
	Editor/Menus/helpMenu.hpp
	Editor/Menus/mainMenu.cpp
	Editor/Menus/mainMenu.hpp
	Editor/Menus/sceneMenu.cpp
	Editor/Menus/sceneMenu.hpp
	Editor/Menus/viewMenu.cpp
	Editor/Menus/viewMenu.hpp
	Editor/Panels/BehaviorEditorPanel.cpp
	Editor/Panels/BehaviorEditorPanel.hpp
	Editor/Panels/NodeInspectorPanel.cpp
	Editor/Panels/NodeInspectorPanel.hpp
	Editor/Panels/RenderScenePanel.cpp
	Editor/Panels/RenderScenePanel.hpp
	Editor/Panels/SceneHierarchyPanel.cpp
	Editor/Panels/SceneHierarchyPanel.hpp
	Editor/Panels/ScenePanel.cpp
	Editor/Panels/ScenePanel.hpp
	Editor/Panels/SimulationPanel.cpp
	Editor/Panels/SimulationPanel.hpp
	Editor/Panels/ToolbarPanel.cpp
	Editor/Panels/ToolbarPanel.hpp
	Exceptions/GLFWException.hpp
	Foundation/GLFWUtils.hpp
	Foundation/JSONUtils.hpp
	Main.cpp
	Rendering/GLFWVulkanSurface.cpp
	Rendering/GLFWVulkanSurface.hpp
	Rendering/GLFWWindow.cpp
	Rendering/GLFWWindow.hpp
	Rendering/ImGUILayer.cpp
	Rendering/ImGUILayer.hpp
	Rendering/Layer.cpp
	Rendering/Layer.hpp
	Simulation/SimulationLayer.cpp
	Simulation/SimulationLayer.hpp
	Simulation/Systems/GLFWSystem.cpp
	Simulation/Systems/GLFWSystem.hpp
	Simulation/Systems/GLFWVulkanSystem.cpp
	Simulation/Systems/GLFWVulkanSystem.hpp
	Simulation/Systems/GLFWWindowSystem.cpp
	Simulation/Systems/GLFWWindowSystem.hpp
	Simulation/Systems/InputSystem.cpp
	Simulation/Systems/InputSystem.hpp
)

add_library(
    ${CRIMILD_LIBRARY_NAME}
    ${CRIMILD_LIBRARY_SOURCE_FILES}
)

target_include_directories( 
	${CRIMILD_LIBRARY_NAME} 
	PUBLIC . 
	PRIVATE ${Crimild_SOURCE_DIR}/third-party/glfw/include
	PRIVATE ${Crimild_SOURCE_DIR}/third-party/json/include
)

target_link_libraries( 
	${CRIMILD_LIBRARY_NAME} 
	PRIVATE crimild_core 
	PRIVATE crimild_vulkan 
	PRIVATE crimild_imgui
	PRIVATE crimild_import
	PRIVATE crimild_stb
	PRIVATE glfw
)

target_compile_features( ${CRIMILD_LIBRARY_NAME} PUBLIC cxx_std_20 )
