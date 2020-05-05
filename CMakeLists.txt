cmake_minimum_required(VERSION 3.0)

project(Boxes)

# SFML
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/ ${CMAKE_MODULE_PATH})
find_package(SFML 2 COMPONENTS graphics window system REQUIRED)

include_directories(${SFML_INCLUDE_DIR})

# Box2D
add_subdirectory(Box2D)
include_directories(${BOX2D_INCLUDE_DIR})

# Pre-build
add_definitions(-std=c++1y)
file(GLOB Source "src/*.*pp")

configure_file(src/Config.hpp.in ${CMAKE_BINARY_DIR}/Config.hpp)
include_directories(${CMAKE_BINARY_DIR})

# Add executable
add_executable(Boxes ${Source})

# Link all libraries
target_link_libraries(Boxes ${BOX2D_LIBRARY} ${SFML_LIBRARIES})
