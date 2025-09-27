cmake_minimum_required(VERSION 3.10)
project(chess C)
set(CMAKE_C_STANDARD 99)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Enable GCC/Clang warnings
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra)
endif()

file(GLOB SRCS src/*.c)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(chess ${SRCS})
