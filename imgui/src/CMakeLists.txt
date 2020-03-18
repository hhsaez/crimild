SET( CRIMILD_INCLUDE_DIRECTORIES
	 ${CRIMILD_INCLUDE_DIRECTORIES}
	 ${CRIMILD_SOURCE_DIR}/core/src
	 ${CRIMILD_SOURCE_DIR}/third-party/imgui
	 ${CRIMILD_SOURCE_DIR}/third-party/imgui/examples
)

SET( CRIMILD_LIBRARY_LINK_LIBRARIES
	 crimild_core
)

SET( CRIMILD_LIBRARY_ADDITIONAL_SOURCE_FILES
	 ${CRIMILD_LIBRARY_ADDITIONAL_SOURCE_FILES}
	 ${CRIMILD_SOURCE_DIR}/third-party/imgui/imgui.cpp
	 ${CRIMILD_SOURCE_DIR}/third-party/imgui/imgui_demo.cpp
	 ${CRIMILD_SOURCE_DIR}/third-party/imgui/imgui_draw.cpp
	 ${CRIMILD_SOURCE_DIR}/third-party/imgui/imgui_widgets.cpp
)

INCLUDE( ModuleBuildLibrary )

