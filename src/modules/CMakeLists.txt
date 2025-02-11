cmake_minimum_required(VERSION 3.16)
project(AutoPilot LANGUAGES CXX)

add_subdirectory(attitude)
add_subdirectory(radio)
add_subdirectory(autopilot)
add_subdirectory(mixer)
add_subdirectory(motor)
add_subdirectory(constants)

add_custom_target(default_target ALL DEPENDS attitude radio autopilot mixer motor constants)