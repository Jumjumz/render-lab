cmake_minimum_required(VERSION 3.10)
project(WireFrameRender VERSION 0.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE COMMANDS ON)

set(SOURCES
src/main.cpp
src/shapes/triangle.cpp
src/shapes/triangle.hpp
src/window/window.cpp
src/window/window.h)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
"${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(${PROJECT_NAME} PRIVATE
Vulkan::Vulkan
SDL2::SDL2)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Configuring for Debug mode...")
  if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
  else()
    target_compile_options(${PROJECT_NAME} PRIVATE -g -O0 -Wall)
  endif()
endif()
