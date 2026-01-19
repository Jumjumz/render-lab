cmake_minimum_required(VERSION 3.10)
project(WireFrameRenderer VERSION 0.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE COMMANDS ON)

find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)

add_executable("${PROJECT_NAME}" src/main.cpp)

target_include_directories("${PROJECT_NAME}" PRIVATE
${Vulkan_INCLUDE_DIRS}
${SDL2_INCLUDE_DIRS})

target_link_libraries("${PROJECT_NAME}" PRIVATE
Vulkan::Vulkan
SDL2::SDL2)

