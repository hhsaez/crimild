SET( CRIMILD_INCLUDE_DIRECTORIES
	 ${CRIMILD_INCLUDE_DIRECTORIES}
	 ${CRIMILD_SOURCE_DIR}/core/src
	 ${CRIMILD_SOURCE_DIR}/third-party/imgui
	 ${CRIMILD_SOURCE_DIR}/third-party/imgui/examples
	 ${CRIMILD_SOURCE_DIR}/third-party/imguizmo
	 ${CRIMILD_SOURCE_DIR}/third-party/imguizmo/examples
	 ${CRIMILD_SOURCE_DIR}/third-party/ImGuiFileDialog
)

SET( CRIMILD_LIBRARY_LINK_LIBRARIES
	 crimild_core
)

SET( CRIMILD_LIBRARY_ADDITIONAL_SOURCE_FILES
	 ${CRIMILD_LIBRARY_ADDITIONAL_SOURCE_FILES}
	 ${CRIMILD_SOURCE_DIR}/third-party/imgui/imgui.cpp
	 ${CRIMILD_SOURCE_DIR}/third-party/imgui/imgui_tables.cpp
	 ${CRIMILD_SOURCE_DIR}/third-party/imgui/imgui_demo.cpp
	 ${CRIMILD_SOURCE_DIR}/third-party/imgui/imgui_draw.cpp
	 ${CRIMILD_SOURCE_DIR}/third-party/imgui/imgui_widgets.cpp
	 ${CRIMILD_SOURCE_DIR}/third-party/imguizmo/ImGuizmo.cpp
	 ${CRIMILD_SOURCE_DIR}/third-party/imguizmo/GraphEditor.cpp
	 ${CRIMILD_SOURCE_DIR}/third-party/ImGuiFileDialog/ImGuiFileDialog.cpp
)

INCLUDE( ModuleBuildLibrary )

