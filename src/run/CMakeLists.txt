cmake_minimum_required(VERSION 3.16)
project(AutoPilot LANGUAGES CXX)

set(SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/run.cpp")

add_executable(run ${SOURCE_FILE})

target_link_libraries(run PUBLIC pthread)