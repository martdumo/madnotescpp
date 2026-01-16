cmake_minimum_required(VERSION 3.16)

project(MadNotesCpp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core Network)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Include Directories
include_directories(include include/core include/ui include/editor include/utils)

# Source Files (Globbing for initial setup, manual list recommended for prod)
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/ui/*.cpp" "src/editor/*.cpp" "src/core/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h" "include/ui/*.h" "include/editor/*.h" "include/core/*.h")

# Executable
add_executable(MadNotesCpp ${SOURCES} ${HEADERS})

target_link_libraries(MadNotesCpp PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core Qt6::Network)

# Windows Subsystem (No console window in release)
if(WIN32)
    set_target_properties(MadNotesCpp PROPERTIES WIN32_EXECUTABLE ON)
endif()
