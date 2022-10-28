crimild_trace()

set(
    CRIMILD_LIBRARY_NAME
    crimild_imgui
)

set( 
    CRIMILD_LIBRARY_SOURCE_FILES 
	Crimild_ImGUI.hpp
	Foundation/ImGUIUtils.hpp
	Rendering/Operations/ImGUIOperations.hpp
	Rendering/Operations/ImGUIOperations_overlayUI.cpp
	Rendering/Operations/ImGUIOperations_renderUI.cpp
	Simulation/Systems/ImGUISystem.cpp
	Simulation/Systems/ImGUISystem.hpp
	 ${Crimild_SOURCE_DIR}/third-party/imgui/imgui.cpp
	 ${Crimild_SOURCE_DIR}/third-party/imgui/imgui_tables.cpp
	 ${Crimild_SOURCE_DIR}/third-party/imgui/imgui_demo.cpp
	 ${Crimild_SOURCE_DIR}/third-party/imgui/imgui_draw.cpp
	 ${Crimild_SOURCE_DIR}/third-party/imgui/imgui_widgets.cpp
	 ${Crimild_SOURCE_DIR}/third-party/imguizmo/ImGuizmo.cpp
	 ${Crimild_SOURCE_DIR}/third-party/imguizmo/GraphEditor.cpp
	 ${Crimild_SOURCE_DIR}/third-party/ImGuiFileDialog/ImGuiFileDialog.cpp
)

add_library(
    ${CRIMILD_LIBRARY_NAME}
    ${CRIMILD_LIBRARY_SOURCE_FILES}
)

target_include_directories( 
	${CRIMILD_LIBRARY_NAME} 
	PUBLIC . 
	PUBLIC ${Crimild_SOURCE_DIR}/third-party/imgui
	PRIVATE ${Crimild_SOURCE_DIR}/third-party/imgui/examples
	PUBLIC ${Crimild_SOURCE_DIR}/third-party/imguizmo
	PRIVATE ${Crimild_SOURCE_DIR}/third-party/imguizmo/examples
	PUBLIC ${Crimild_SOURCE_DIR}/third-party/ImGuiFileDialog
)

target_link_libraries( 
	${CRIMILD_LIBRARY_NAME} 
	PRIVATE crimild_core 
)

target_compile_features( ${CRIMILD_LIBRARY_NAME} PUBLIC cxx_std_20 )
