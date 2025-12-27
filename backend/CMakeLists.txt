cmake_minimum_required(VERSION 3.15)
project(WizardMerge VERSION 0.1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find dependencies via Conan
find_package(Drogon CONFIG QUIET)
find_package(GTest QUIET)
find_package(CURL QUIET)

# Library sources
add_library(wizardmerge
    src/merge/three_way_merge.cpp
    src/git/github_client.cpp
)

target_include_directories(wizardmerge
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link CURL if available
if(CURL_FOUND)
    target_link_libraries(wizardmerge PUBLIC CURL::libcurl)
endif()

# Executable (only if Drogon is found)
if(Drogon_FOUND)
    add_executable(wizardmerge-cli
        src/main.cpp
        src/controllers/MergeController.cc
        src/controllers/PRController.cc
    )

    target_link_libraries(wizardmerge-cli PRIVATE wizardmerge Drogon::Drogon)
    
    install(TARGETS wizardmerge-cli
        RUNTIME DESTINATION bin
    )
    
    message(STATUS "Drogon found - building HTTP server")
else()
    message(WARNING "Drogon not found - skipping HTTP server build. Install Drogon to build the server.")
endif()

# Tests (if GTest is available)
if(GTest_FOUND)
    enable_testing()
    add_executable(wizardmerge-tests
        tests/test_three_way_merge.cpp
        tests/test_github_client.cpp
    )
    target_link_libraries(wizardmerge-tests PRIVATE wizardmerge GTest::gtest_main)
    
    include(GoogleTest)
    gtest_discover_tests(wizardmerge-tests)
endif()

# Install targets
install(TARGETS wizardmerge
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)
