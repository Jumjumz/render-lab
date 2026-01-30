cmake_minimum_required(VERSION 3.10)
project(RenderLab VERSION 0.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
src/main.cpp
src/renderer/vulkan.cpp
src/renderer/vulkan.h
src/window/window.cpp
src/window/window.h)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()


find_program(SLANGC_EXE slangc)

if(NOT SLANGC_EXE)
  message(FATAL_ERROR "slangc compiler not found!")
endif()

find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)

function(add_slang_shader_target TARGET)
  cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})

  set(SHADERS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
  file(MAKE_DIRECTORY "${SHADERS_OUTPUT_DIR}")

  add_custom_command(
    OUTPUT "${SHADERS_OUTPUT_DIR}/slang.spv"
    COMMAND ${SLANGC_EXE} "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_SOURCES}"
            -target spirv
            -profile spirv_1_4
            -emit-spirv-directly
            -fvk-use-entrypoint-name
            -entry vertMain -entry fragMain
            -o "${SHADERS_OUTPUT_DIR}/slang.spv"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_SOURCES}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Compiling Slang Shaders"
    VERBATIM
  )
  add_custom_target(${TARGET} DEPENDS ${SHADERS_OUTPUT_DIR}/slang.spv)
endfunction()

add_slang_shader_target(shader_gen SOURCES src/shaders/shader.slang)

add_executable(${PROJECT_NAME} ${SOURCES})
add_dependencies(${PROJECT_NAME} shader_gen)

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
