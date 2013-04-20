CMAKE_MINIMUM_REQUIRED( VERSION 2.8.10 FATAL_ERROR )

# Enable C++11 features
SET( CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ -U__STRICT_ANSI__" )

PROJECT( CRIMILD )

# Register custom modules
SET ( CMAKE_MODULE_PATH "${CRIMILD_SOURCE_DIR}/CMakeTools;${CMAKE_MODULE_PATH}" )

# Configuration options
OPTION( CRIMILD_ENABLE_TESTS "Would you like to enable unit tests?" OFF )

# Enable unit testing 
ENABLE_TESTING()

# Add sources
ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( examples )
ADD_SUBDIRECTORY( tests )

