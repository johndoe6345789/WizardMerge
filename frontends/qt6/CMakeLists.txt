cmake_minimum_required(VERSION 3.16)
project(wizardmerge-qt6 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 configuration
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 COMPONENTS Core Widgets Quick Network QUIET)

if(NOT Qt6_FOUND)
    message(WARNING "Qt6 not found. Skipping Qt6 frontend build.")
    message(WARNING "Install Qt6 to build the Qt6 frontend:")
    message(WARNING "  - Ubuntu/Debian: sudo apt-get install qt6-base-dev qt6-declarative-dev")
    message(WARNING "  - macOS: brew install qt@6")
    message(WARNING "  - Windows: Download from https://www.qt.io/download")
    return()
endif()

# Source files
set(SOURCES
    src/main.cpp
)

# QML files
set(QML_FILES
    qml/main.qml
)

# Create executable
qt_add_executable(wizardmerge-qt6
    ${SOURCES}
)

# Add QML module
qt_add_qml_module(wizardmerge-qt6
    URI WizardMerge
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

# Link Qt libraries
target_link_libraries(wizardmerge-qt6 PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    Qt6::Network
)

# Include directories
target_include_directories(wizardmerge-qt6 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Install target
install(TARGETS wizardmerge-qt6
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

# Platform-specific settings
if(WIN32)
    set_target_properties(wizardmerge-qt6 PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(APPLE)
    set_target_properties(wizardmerge-qt6 PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()

message(STATUS "Qt6 frontend configured successfully")
