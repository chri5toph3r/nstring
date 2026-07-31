cmake_minimum_required(VERSION 4.1.2)
project(nstring)

set(CMAKE_CXX_STANDARD 17)

include_directories(inc)

add_executable(nstring src/main.c src/logger.c src/nstring.c
)