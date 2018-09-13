# System configuration
IF ( ${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten" )
  	SET( CRIMILD_ENABLE_EMSCRIPTEN ON )
	ADD_DEFINITIONS( -DCRIMILD_PLATFORM_EMSCRIPTEN=1 )
ELSE ()
  	SET( CRIMILD_ENABLE_EMSCRIPTEN OFF )
ENDIF ()
  
IF ( APPLE )
  	 # Enable C++11 features
	 SET( CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -U__STRICT_ANSI__ -fvisibility=hidden -fvisibility-inlines-hidden" )

	 SET( CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x" )
	 SET( CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++" )

	 SET( CMAKE_MACOSX_RPATH 1 )
ELSE ( APPLE )
  	   IF ( CRIMILD_ENABLE_EMSCRIPTEN )
		  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static -U__STRICT_ANSI__" )
	   ELSE ()
		  SET( CMAKE_CXX_FLAGS "-std=c++11 -static-libgcc -static-libstdc++ -static -U__STRICT_ANSI__ -fvisibility=hidden -fvisibility-inlines-hidden" )
	   ENDIF ()
ENDIF ( APPLE )
