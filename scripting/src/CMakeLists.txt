SET( CRIMILD_INCLUDE_DIRECTORIES 
	${CRIMILD_SOURCE_DIR}/core/src 
	${CRIMILD_SOURCE_DIR}/third-party/lua/src )

SET( CRIMILD_LIBRARY_LINK_LIBRARIES 
	crimild_core )

# Add Lua sources as part of the library
SET( 
	CRIMILD_LIBRARY_ADDITIONAL_SOURCE_FILES 
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lapi.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lcode.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lctype.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/ldebug.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/ldo.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/ldump.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lfunc.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lgc.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/llex.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lmem.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lobject.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lopcodes.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lparser.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lstate.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lstring.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/ltable.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/ltm.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lundump.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lvm.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lzio.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lauxlib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lbaselib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lbitlib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lcorolib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/ldblib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/liolib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lmathlib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/loslib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lstrlib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/ltablib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/loadlib.c 
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/lutf8lib.c 
	${CRIMILD_SOURCE_DIR}/third-party/lua/src/linit.c )

IF ( CRIMILD_ENABLE_IMPORT )
	SET( CRIMILD_LIBRARY_LINK_LIBRARIES 
		${CRIMILD_LIBRARY_LINK_LIBRARIES} 
		crimild_import )

	SET( CRIMILD_INCLUDE_DIRECTORIES 
		${CRIMILD_INCLUDE_DIRECTORIES}
		${CRIMILD_SOURCE_DIR}/import/src
		${Assimp_SOURCE_DIR}/include
  		${Assimp_SOURCE_DIR}/code )
ENDIF ( CRIMILD_ENABLE_IMPORT )

IF ( CRIMILD_ENABLE_PHYSICS )
	SET( CRIMILD_LIBRARY_LINK_LIBRARIES 
		 ${CRIMILD_LIBRARY_LINK_LIBRARIES} 
		 crimild_physics )

	SET( CRIMILD_INCLUDE_DIRECTORIES 
		 ${CRIMILD_INCLUDE_DIRECTORIES} 
		 ${CRIMILD_SOURCE_DIR}/physics/src
		 ${CRIMILD_SOURCE_DIR}/third-party/bullet-2.82-r2704/src )
ENDIF ( CRIMILD_ENABLE_PHYSICS )

INCLUDE( ModuleBuildLibrary )
