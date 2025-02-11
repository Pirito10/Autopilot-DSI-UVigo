cmake_minimum_required(VERSION 3.16)
project(AutoPilot LANGUAGES CXX)

include_directories(/data/usr/include)

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(file ${files})
    get_filename_component(target ${file} NAME_WE)

    add_executable(${target} EXCLUDE_FROM_ALL ${file})

    target_compile_definitions(${target} PUBLIC ZENOHCXX_ZENOHC)

    target_link_libraries(${target} PUBLIC zenohc)

    set_property(TARGET ${target} PROPERTY LANGUAGE CXX)

    set_property(TARGET ${target}  PROPERTY CXX_STANDARD 17)
endforeach()