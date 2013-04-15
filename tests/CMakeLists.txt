IF ( NOT CRIMILD_ENABLE_TESTS )
	MESSAGE( "-- Skipping unit tests" )
	RETURN()
ENDIF ( NOT CRIMILD_ENABLE_TESTS )

MESSAGE( "-- Configuring unit tests" )

IF ( NOT EXISTS ${CRIMILD_SOURCE_DIR}/third-party/gtest-1.6.0 )
	MESSAGE( "   Unpacking GTest sources" )

	EXECUTE_PROCESS ( 
		COMMAND ${CMAKE_COMMAND} 
		-E tar -x
		${CRIMILD_SOURCE_DIR}/third-party/gtest-1.6.0.zip
		WORKING_DIRECTORY ${CRIMILD_SOURCE_DIR}/third-party/
	)
	
ENDIF ( NOT EXISTS ${CRIMILD_SOURCE_DIR}/third-party/gtest-1.6.0 )

ADD_DEFINITIONS( -DGTEST_LANG_CXX11=1 )
ADD_DEFINITIONS( -DGTEST_USE_OWN_TR1_TUPLE=1 )

ADD_SUBDIRECTORY( ${CRIMILD_SOURCE_DIR}/third-party/gtest-1.6.0 "${CMAKE_CURRENT_BINARY_DIR}/../third-party/gtest-1.6.0" )

FILE( GLOB_RECURSE CRIMILD_TESTS_HEADER_FILES "${CRIMILD_SOURCE_DIR}/tests/*Test.hpp" )
FILE( GLOB_RECURSE CRIMILD_TESTS_SOURCE_FILES "${CRIMILD_SOURCE_DIR}/tests/*Test.cpp")

# set default dependencies
SET( CRIMILD_TESTS_DEPENDENCIES
	gtest 
	gtest_main
)

INCLUDE_DIRECTORIES(
	${CRIMILD_SOURCE_DIR}/src
	${gtest_SOURCE_DIR}/include 
	${gtest_SOURCE_DIR}
)

FOREACH( test ${CRIMILD_TESTS_SOURCE_FILES} )
	GET_FILENAME_COMPONENT( TestName ${test} NAME_WE)
	MESSAGE( "   Adding test: " ${TestName} )
	SOURCE_GROUP( Crimild\\Tests\\${TestName} FILES ${test} )
	ADD_EXECUTABLE( ${TestName} TestRunner.cpp ${test} )
	TARGET_LINK_LIBRARIES( ${TestName} ${CRIMILD_TESTS_DEPENDENCIES} )
	ADD_TEST( ${TestName} ${TestName} )
ENDFOREACH()

