cmake_minimum_required(VERSION 3.16)
project(examplecpp LANGUAGES C CXX)

include_directories(/home/pi/lib/Navio)

link_directories(/home/pi/lib/Navio)

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(file ${files})
    get_filename_component(target ${file} NAME_WE)

    add_executable(${target} EXCLUDE_FROM_ALL ${file})

    target_link_libraries(${target} PUBLIC navio)

    set_property(TARGET ${target} PROPERTY LANGUAGE CXX)

    set_property(TARGET ${target}  PROPERTY CXX_STANDARD 17)

endforeach()
