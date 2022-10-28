include( CTest )
include( GoogleTest )

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory( "${PROJECT_SOURCE_DIR}/third-party/googletest" )

# Keeps CACHE cleaner
mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

# Keep IDEs folders clean
set_target_properties( gtest PROPERTIES FOLDER extern )
set_target_properties( gtest_main PROPERTIES FOLDER extern )
set_target_properties( gmock PROPERTIES FOLDER extern )
set_target_properties( gmock_main PROPERTIES FOLDER extern )
