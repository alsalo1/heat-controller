cmake_minimum_required(VERSION 3.1)

project(heat-controller C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -std=c99 -D_XOPEN_SOURCE=500")

include_directories(include)

add_subdirectory(src)
