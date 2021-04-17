cmake_minimum_required(VERSION 3.17)
project(g8_gipfel C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(g8_gipfel
        crypto.c
        dummy_chat.c
        utils.c)
