cmake_minimum_required(VERSION 3.15)
project(WizardMerge VERSION 0.1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find dependencies via Conan
find_package(GTest QUIET)

# Library sources
add_library(wizardmerge
    src/merge/three_way_merge.cpp
)

target_include_directories(wizardmerge
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Executable
add_executable(wizardmerge-cli
    src/main.cpp
)

target_link_libraries(wizardmerge-cli PRIVATE wizardmerge)

# Tests (if GTest is available)
if(GTest_FOUND)
    enable_testing()
    add_executable(wizardmerge-tests
        tests/test_three_way_merge.cpp
    )
    target_link_libraries(wizardmerge-tests PRIVATE wizardmerge GTest::gtest_main)
    
    include(GoogleTest)
    gtest_discover_tests(wizardmerge-tests)
endif()

# Install targets
install(TARGETS wizardmerge wizardmerge-cli
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)
