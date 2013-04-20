SET( CRIMILD_LIBRARY_NAME crimild )

INCLUDE( MacroAddSources )

ADD_SOURCES( ${CRIMILD_SOURCE_DIR}/src *.hpp )
ADD_SOURCES( ${CRIMILD_SOURCE_DIR}/src *.cpp )

INCLUDE( ModuleBuildLibrary )
