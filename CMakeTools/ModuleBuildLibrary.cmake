# This module configures how to build a library
# The following arguments are valid:
# 	CRIMILD_LIBRARY_NAME: (Required) Name of the library
#	CRIMILD_LIBRARY_SOURCE_FILES: (Optional) Collection of files that need to be included as part of the library
#	CRIMILD_LIBRARY_LINK_LIBRARIES: (Optional) External libraries that need to be linked with this one
#	CRIMILD_LIBRARY_DEPENDENCIES: (Optional) Any dependencies that are required in order to build the library
#	CRIMILD_INCLUDE_DIRECTORIES: (Optional) Additional include directories for dependencies
#	CRIMILD_LINK_DIRECTORIES: (Optional) Additional link directories for dependencies

INCLUDE( MacroAddSources )

MESSAGE( "   Adding sources" )

ADD_SOURCES( ${CRIMILD_SOURCE_DIR}/${CRIMILD_LIBRARY_NAME}/src *.hpp )
ADD_SOURCES( ${CRIMILD_SOURCE_DIR}/${CRIMILD_LIBRARY_NAME}/src *.cpp )

# Set library output path
SET ( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib )

# Add any additional include directories if required
INCLUDE_DIRECTORIES( ${CRIMILD_INCLUDE_DIRECTORIES} )

# Add additional linkage directories if needed
LINK_DIRECTORIES( ${CRIMILD_SOURCE_DIR}/lib ${CRIMILD_LINK_DIRECTORIES} )

# By default, libraries are build as STATIC
SET( CRIMILD_LIBRARY_DISTRIBUTION STATIC )

# Add a library subproject for Crimild
ADD_LIBRARY( crimild_${CRIMILD_LIBRARY_NAME} 
			 ${CRIMILD_LIBRARY_DISTRIBUTION} 
			 ${CRIMILD_LIBRARY_SOURCE_FILES} )
			 
# Link any dependencies for the project
IF ( CRIMILD_LIBRARY_LINK_LIBRARIES )
	TARGET_LINK_LIBRARIES( crimild_${CRIMILD_LIBRARY_NAME} ${CRIMILD_LIBRARY_LINK_LIBRARIES} )
ENDIF ( CRIMILD_LIBRARY_LINK_LIBRARIES )

IF ( CRIMILD_LIBRARY_DEPENDENCIES )
	ADD_DEPENDENCIES( crimild_${CRIMILD_LIBRARY_NAME} ${CRIMILD_LIBRARY_DEPENDENCIES} )
ENDIF ( CRIMILD_LIBRARY_DEPENDENCIES )
