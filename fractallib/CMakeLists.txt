cmake_minimum_required(VERSION 3.8.0)

project(fractallib)

include(${PROJECT_SOURCE_DIR}/../common.cmake)

set(header_files
        axefx.h)

set(source_files
        axefx.c)

add_library(fractallib ${header_files} ${source_files})
