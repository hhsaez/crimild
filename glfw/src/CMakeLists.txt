crimild_trace()

find_package( Vulkan REQUIRED )

add_library( crimild_glfw_base INTERFACE )
target_include_directories( 
	crimild_glfw_base
	INTERFACE ${PROJECT_SOURCE_DIR}/third-party/glfw/include
	INTERFACE ${PROJECT_SOURCE_DIR}/third-party/json/include
	INTERFACE ${Vulkan_INCLUDE_DIRS} 
	INTERFACE ${PROJECT_SOURCE_DIR}/third-party/stb
	INTERFACE ${PROJECT_SOURCE_DIR}/third-party/tinygltf
)
target_link_libraries( 
	crimild_glfw_base
	INTERFACE Crimild::Core 
	INTERFACE Crimild::Vulkan
	INTERFACE Crimild::ImGui
	INTERFACE glfw
)

add_library( crimild_glfw OBJECT )

target_sources(
    crimild_glfw

	PUBLIC
	Coding/JSONDecoder.hpp
	Coding/JSONEncoder.hpp
	Crimild_GLFW.hpp
	Editor/EditorLayer.hpp
	Editor/EditorProject.hpp
	Editor/Layout.hpp
	Editor/EditorState.hpp
	Editor/EditorUtils.hpp
	Editor/Menus/editMenu.hpp
	Editor/Menus/fileMenu.hpp
	Editor/Menus/helpMenu.hpp
	Editor/Menus/mainMenu.hpp
	Editor/Menus/sceneMenu.hpp
	Editor/Menus/viewMenu.hpp
	Editor/Panels/BehaviorEditorPanel.hpp
	Editor/Panels/EditorProjectPanel.hpp
	Editor/Panels/NodeInspectorPanel.hpp
	Editor/Panels/RenderScenePanel.hpp
	Editor/Panels/SceneHierarchyPanel.hpp
	Editor/Panels/ScenePanel.hpp
	Editor/Panels/SimulationPanel.hpp
	Editor/Panels/ToolbarPanel.hpp
	Exceptions/GLFWException.hpp
	Foundation/GLFWUtils.hpp
	Foundation/JSONUtils.hpp
	Rendering/GLFWVulkanSurface.hpp
	Rendering/GLFWWindow.hpp
	Rendering/ImGUILayer.hpp
	Rendering/Layer.hpp
	Simulation/SimulationLayer.hpp
	Simulation/Systems/GLFWSystem.hpp
	Simulation/Systems/GLFWVulkanSystem.hpp
	Simulation/Systems/GLFWWindowSystem.hpp
	Simulation/Systems/InputSystem.hpp

	PRIVATE
	Coding/JSONDecoder.cpp
	Coding/JSONEncoder.cpp
	Crimild_GLFW.cpp
	Editor/EditorLayer.cpp
	Editor/EditorProject.cpp
	Editor/Layout.cpp
	Editor/EditorState.cpp
	Editor/EditorUtils.cpp
	Editor/Menus/editMenu.cpp
	Editor/Menus/fileMenu.cpp
	Editor/Menus/helpMenu.cpp
	Editor/Menus/mainMenu.cpp
	Editor/Menus/sceneMenu.cpp
	Editor/Menus/viewMenu.cpp
	Editor/Panels/BehaviorEditorPanel.cpp
	Editor/Panels/EditorProjectPanel.cpp
	Editor/Panels/NodeInspectorPanel.cpp
	Editor/Panels/RenderScenePanel.cpp
	Editor/Panels/SceneHierarchyPanel.cpp
	Editor/Panels/ScenePanel.cpp
	Editor/Panels/SimulationPanel.cpp
	Editor/Panels/ToolbarPanel.cpp
	Foundation/STBUtils.cpp
	Foundation/STBUtils.hpp
	Importers/GLTFImporter.cpp
	Importers/GLTFImporter.hpp
	Rendering/GLFWVulkanSurface.cpp
	Rendering/GLFWWindow.cpp
	Rendering/ImGUILayer.cpp
	Rendering/Layer.cpp
	Rendering/STBImageManager.cpp
	Rendering/STBImageManager.hpp
	Simulation/SimulationLayer.cpp
	Simulation/Systems/GLFWSystem.cpp
	Simulation/Systems/GLFWVulkanSystem.cpp
	Simulation/Systems/GLFWWindowSystem.cpp
	Simulation/Systems/InputSystem.cpp
)

target_include_directories( crimild_glfw PUBLIC . )
target_link_libraries( crimild_glfw PRIVATE crimild_glfw_base )
add_library( Crimild::GLFW ALIAS crimild_glfw )

add_library( crimild_glfw_main OBJECT )
target_sources( crimild_glfw_main PRIVATE Main.cpp )
target_link_libraries( 
	crimild_glfw_main 
	PRIVATE crimild_glfw_base
	PRIVATE crimild_glfw
)
add_library( Crimild::GLFW::Main ALIAS crimild_glfw_main )

# Keep IDEs folders clean
set_target_properties( glfw PROPERTIES FOLDER extern )
