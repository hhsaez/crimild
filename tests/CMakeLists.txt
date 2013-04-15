MESSAGE( "-- Configuring unit tests" )

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

