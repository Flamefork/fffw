cmake_minimum_required(VERSION 3.8.0)

include(common.cmake)
include(macros.cmake)

add_library_directory(tblib)
add_library_directory(lcdlib)
add_library_directory(fractallib)

project(fffw)

add_executable(fffw
        src/axe_defs.h
        src/main.c)

add_library_dependency(fffw tblib)
add_library_dependency(fffw lcdlib)
add_library_dependency(fffw fractallib)

#flash(fffw 9600 /dev/cu.usbserial-AI1WX4XP)
