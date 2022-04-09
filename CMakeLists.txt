cmake_minimum_required(VERSION 3.18)
project(spim C)

set(CMAKE_C_STANDARD 99)

add_executable(spim spimcore.c)
target_sources(spim PRIVATE project.c)