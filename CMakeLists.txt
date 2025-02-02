cmake_minimum_required(VERSION 3.13)
project(CHIP8_EMULATOR)
set(CMAKE_C_STANDARD 23)

find_package(SDL3 REQUIRED)
include_directories(${SDL3_INCLUDE_DIRS})

file(GLOB sources
      "./include/*.h"
      "./src/*.c"
)

add_executable(chip8 ${sources})
target_link_libraries(chip8 ${SDL3_LIBRARIES})
