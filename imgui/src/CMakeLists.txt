crimild_trace()

add_library( crimild_imgui OBJECT )

target_sources(
    crimild_imgui
	
	PUBLIC
	Crimild_ImGUI.hpp
	Foundation/ImGUIUtils.hpp
	Rendering/Operations/ImGUIOperations.hpp
	Simulation/Systems/ImGUISystem.hpp

	PRIVATE
	Rendering/Operations/ImGUIOperations_overlayUI.cpp
	Rendering/Operations/ImGUIOperations_renderUI.cpp
	Simulation/Systems/ImGUISystem.cpp
	${PROJECT_SOURCE_DIR}/third-party/imgui/imgui.cpp
	${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_tables.cpp
	${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_draw.cpp
	${PROJECT_SOURCE_DIR}/third-party/imgui/imgui_widgets.cpp
	${PROJECT_SOURCE_DIR}/third-party/imguizmo/ImGuizmo.cpp
	${PROJECT_SOURCE_DIR}/third-party/imguizmo/GraphEditor.cpp
	${PROJECT_SOURCE_DIR}/third-party/ImGuiFileDialog/ImGuiFileDialog.cpp
)

target_include_directories( 
	crimild_imgui
	PUBLIC . 
	PUBLIC ${PROJECT_SOURCE_DIR}/third-party/imgui
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imgui/examples
	PUBLIC ${PROJECT_SOURCE_DIR}/third-party/imguizmo
	PRIVATE ${PROJECT_SOURCE_DIR}/third-party/imguizmo/examples
	PUBLIC ${PROJECT_SOURCE_DIR}/third-party/ImGuiFileDialog
)

target_link_libraries( 
	crimild_imgui 
	PRIVATE Crimild::Core
	PRIVATE Crimild::Vulkan
)

add_library( Crimild::ImGui ALIAS crimild_imgui )
