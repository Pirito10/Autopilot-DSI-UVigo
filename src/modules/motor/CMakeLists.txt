cmake_minimum_required(VERSION 3.16)
project(AutoPilot LANGUAGES CXX)

include_directories(/data/usr/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/Navio)

link_directories(${CMAKE_SOURCE_DIR}/lib/Navio)

set(SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/motor.cpp")

add_executable(motor ${SOURCE_FILE})

target_compile_definitions(motor PUBLIC ZENOHCXX_ZENOHC)
target_link_libraries(motor PUBLIC zenohc navio)

set_property(TARGET motor PROPERTY LANGUAGE CXX)
set_property(TARGET motor PROPERTY CXX_STANDARD 17)