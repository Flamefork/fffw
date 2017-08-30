cmake_minimum_required(VERSION 2.6.0)

include(${PROJECT_SOURCE_DIR}/common.cmake)

project(fractallib)

set(header_files
        axefx.h)

set(source_files
        axefx.c)

add_library(fractallib ${header_files} ${source_files})
