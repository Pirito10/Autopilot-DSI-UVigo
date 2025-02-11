cmake_minimum_required(VERSION 3.16)
project(AutoPilot LANGUAGES CXX)

include_directories(/data/usr/include)

set(SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/constants.cpp")

add_executable(constants ${SOURCE_FILE})

target_compile_definitions(constants PUBLIC ZENOHCXX_ZENOHC)
target_link_libraries(constants PUBLIC zenohc)

set_property(TARGET constants PROPERTY LANGUAGE CXX)
set_property(TARGET constants PROPERTY CXX_STANDARD 17)