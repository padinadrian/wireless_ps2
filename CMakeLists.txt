# Top-level CMake file
cmake_minimum_required(VERSION 3.5)
project(wireless_ps2)

# Version information
set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 1)
set(${PROJECT_NAME}_VERSION_BUGFIX 0)

# Add git version info and generate version source
# Take from https://stackoverflow.com/a/4318642/5179394
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
set(${PROJECT_NAME}_VERSION_FULL
    "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUGFIX}.${GIT_SHA1}")

# Create version number file
file(WRITE "${CMAKE_BINARY_DIR}/version" "${${PROJECT_NAME}_VERSION_FULL}\n")

# Universal settings
include_directories("${CMAKE_SOURCE_DIR}/include")

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Individual projects
add_subdirectory("embedded_tools")
