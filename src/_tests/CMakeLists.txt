cmake_minimum_required(VERSION 3.5)

project(Test_action_parser)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(default_build_type "Debug")
include_directories(SYSTEM "amiga-compatibility")

add_executable(test_action_parser
               test_action_parser.c
               ../rename_counter.c)
