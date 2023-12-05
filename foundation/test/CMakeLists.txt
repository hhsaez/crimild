crimild_trace()

add_executable( crimild_foundation_test )

target_sources( 
    crimild_foundation_test

    PRIVATE
    Common/NamedObjectTest.cpp
    Common/RTTITest.cpp
    Common/VersionTest.cpp
    Containers/ArrayTest.cpp
    Containers/DigraphTest.cpp
    Containers/MapTest.cpp
    Containers/PriorityQueueTest.cpp
    Containers/QueueTest.cpp
    Containers/SetTest.cpp
    Containers/StackTest.cpp
    Policies/PriorityTest.cpp

    PRIVATE
    TestRunner.cpp
)

target_include_directories( crimild_foundation_test PRIVATE . )
target_link_libraries( 
    crimild_foundation_test
    PRIVATE crimild_foundation 
    PRIVATE gtest gmock
)
gtest_discover_tests( 
    crimild_foundation_test
    WORKING_DIRECTORY ${PROJECT_DIR}
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
)
set_target_properties( crimild_foundation_test PROPERTIES FOLDER tests )
