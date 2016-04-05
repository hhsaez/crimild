SET( CRIMILD_INCLUDE_DIRECTORIES 
	${CRIMILD_SOURCE_DIR}/core/src 
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src )

SET( CRIMILD_LIBRARY_LINK_LIBRARIES 
	crimild_core )

# Add Lua sources as part of the library
SET( 
	CRIMILD_LIBRARY_ADDITIONAL_SOURCE_FILES 
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lapi.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lcode.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lctype.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/ldebug.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/ldo.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/ldump.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lfunc.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lgc.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/llex.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lmem.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lobject.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lopcodes.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lparser.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lstate.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lstring.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/ltable.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/ltm.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lundump.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lvm.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lzio.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lauxlib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lbaselib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lbitlib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lcorolib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/ldblib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/liolib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lmathlib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/loslib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/lstrlib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/ltablib.c
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/loadlib.c 
	${CRIMILD_SOURCE_DIR}/third-party/lua-5.2.3/src/linit.c )

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

INCLUDE( ModuleBuildLibrary )
