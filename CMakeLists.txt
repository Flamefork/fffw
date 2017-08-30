cmake_minimum_required(VERSION 2.6.0)

include(common.cmake)

project(fffw)

add_executable(fffw src/main.c)

add_project_library_dependency(fffw tblib)
add_project_library_dependency(fffw lcdlib)
add_project_library_dependency(fffw fractallib)

create_hex(fffw)
