cmake_minimum_required(VERSION 3.16)
project(AutoPilot LANGUAGES CXX)

include_directories(/data/usr/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/Navio)

link_directories(${CMAKE_SOURCE_DIR}/lib/Navio)

set(SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/radio.cpp")

add_executable(radio ${SOURCE_FILE})

target_compile_definitions(radio PUBLIC ZENOHCXX_ZENOHC)
target_link_libraries(radio PUBLIC zenohc navio)

set_property(TARGET radio PROPERTY LANGUAGE CXX)
set_property(TARGET radio PROPERTY CXX_STANDARD 17)