cmake_minimum_required(VERSION 3.16)
project(AutoPilot LANGUAGES CXX)

include_directories(/data/usr/include)

set(SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/mixer.cpp")

add_executable(mixer ${SOURCE_FILE})

target_compile_definitions(mixer PUBLIC ZENOHCXX_ZENOHC)
target_link_libraries(mixer PUBLIC zenohc)

set_property(TARGET mixer PROPERTY LANGUAGE CXX)
set_property(TARGET mixer PROPERTY CXX_STANDARD 17)