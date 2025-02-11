cmake_minimum_required(VERSION 3.16)
project(AutoPilot LANGUAGES CXX)

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(file ${files})
    get_filename_component(target ${file} NAME_WE)

    add_executable(${target} EXCLUDE_FROM_ALL ${file})

    target_link_libraries(run PUBLIC pthread)
endforeach()