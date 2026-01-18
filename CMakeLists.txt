cmake_minimum_required(VERSION 3.10)
project(WireFrameRenderer VERSION 0.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE COMMANDS ON)

find_package(SDL2 REQUIRED)

add_executable(main.out src/main.cpp)

target_link_libraries(main.out SDL2::SDL2)

target_include_directories(main.out PUBLIC ${SDL2_INCLUDE_DIRS})
