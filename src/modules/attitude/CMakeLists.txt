cmake_minimum_required(VERSION 3.16)
project(AutoPilot LANGUAGES CXX)

include_directories(/data/usr/include)
include_directories(${CMAKE_SOURCE_DIR}/lib/RTIMULib/RTIMULib)

link_directories(${CMAKE_SOURCE_DIR}/lib/RTIMULib/Linux/build/RTIMULib)

set(SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/attitude.cpp")

add_executable(attitude ${SOURCE_FILE})

target_compile_definitions(attitude PUBLIC ZENOHCXX_ZENOHC)
target_link_libraries(attitude PUBLIC RTIMULib zenohc)

set_property(TARGET attitude PROPERTY LANGUAGE CXX)
set_property(TARGET attitude PROPERTY CXX_STANDARD 17)