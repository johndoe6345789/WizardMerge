cmake_minimum_required(VERSION 3.15)
project(wizardmerge-cli-frontend VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find libcurl
find_package(CURL QUIET)

if(NOT CURL_FOUND)
    message(WARNING "libcurl not found. Skipping CLI frontend build.")
    message(WARNING "Install libcurl to build the CLI frontend:")
    message(WARNING "  - Ubuntu/Debian: sudo apt-get install libcurl4-openssl-dev")
    message(WARNING "  - macOS: brew install curl")
    message(WARNING "  - Windows: Install via vcpkg or use system curl")
    return()
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/http_client.cpp
    src/file_utils.cpp
)

# Header files
set(HEADERS
    include/http_client.h
    include/file_utils.h
)

# Create executable
add_executable(wizardmerge-cli-frontend
    ${SOURCES}
    ${HEADERS}
)

# Include directories
target_include_directories(wizardmerge-cli-frontend PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CURL_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(wizardmerge-cli-frontend PRIVATE
    ${CURL_LIBRARIES}
)

# Compiler warnings
if(MSVC)
    target_compile_options(wizardmerge-cli-frontend PRIVATE /W4)
else()
    target_compile_options(wizardmerge-cli-frontend PRIVATE -Wall -Wextra -pedantic)
endif()

# Install target
install(TARGETS wizardmerge-cli-frontend
    RUNTIME DESTINATION bin
)

message(STATUS "CLI frontend configured successfully")
