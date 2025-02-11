cmake_minimum_required(VERSION 3.16)
project(AutoPilot LANGUAGES CXX)

include_directories(/data/usr/include)

set(SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/autopilot.cpp")

add_executable(autopilot ${SOURCE_FILE})

target_compile_definitions(autopilot PUBLIC ZENOHCXX_ZENOHC)
target_link_libraries(autopilot PUBLIC zenohc)

set_property(TARGET autopilot PROPERTY LANGUAGE CXX)
set_property(TARGET autopilot PROPERTY CXX_STANDARD 17)