add_executable( crimild_foundation_test )

target_sources( 
    crimild_foundation_test

    PRIVATE common/NamedObjectTest.cpp
    PRIVATE common/RTTITest.cpp
    PRIVATE common/VersionTest.cpp

    PRIVATE containers/ArrayTest.cpp
    PRIVATE containers/DigraphTest.cpp
    PRIVATE containers/MapTest.cpp
    PRIVATE containers/PriorityQueueTest.cpp
    PRIVATE containers/QueueTest.cpp
    PRIVATE containers/SetTest.cpp
    PRIVATE containers/StackTest.cpp

    PRIVATE policies/PriorityTest.cpp

    PRIVATE TestRunner.cpp
)

target_include_directories( crimild_foundation_test PRIVATE . )

target_link_libraries( 
    crimild_foundation_test
    PRIVATE crimild::foundation 
    PRIVATE gtest gmock
)

gtest_discover_tests( 
    crimild_foundation_test
    WORKING_DIRECTORY ${PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)

set_target_properties( crimild_foundation_test PROPERTIES FOLDER tests )
