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
set(WIZARDMERGE_SOURCES
    src/merge/three_way_merge.cpp
    src/git/git_cli.cpp
)

# Add git sources only if CURL is available
if(CURL_FOUND)
    list(APPEND WIZARDMERGE_SOURCES src/git/git_platform_client.cpp)
    message(STATUS "CURL found - including Git platform API client (GitHub & GitLab)")
else()
    message(WARNING "CURL not found - Git platform API features will be unavailable")
endif()

add_library(wizardmerge ${WIZARDMERGE_SOURCES})

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
    set(CLI_SOURCES
        src/main.cpp
        src/controllers/MergeController.cc
    )
    
    # Add PR controller only if CURL is available
    if(CURL_FOUND)
        list(APPEND CLI_SOURCES src/controllers/PRController.cc)
        message(STATUS "CURL found - including PR resolution endpoint")
    endif()
    
    add_executable(wizardmerge-cli ${CLI_SOURCES})

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
    
    set(TEST_SOURCES 
        tests/test_three_way_merge.cpp
        tests/test_git_cli.cpp
    )
    
    # Add github client tests only if CURL is available
    if(CURL_FOUND)
        list(APPEND TEST_SOURCES tests/test_git_platform_client.cpp)
    endif()
    
    add_executable(wizardmerge-tests ${TEST_SOURCES})
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
