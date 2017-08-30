cmake_minimum_required(VERSION 3.8.0)

project(lcdlib)

include(${PROJECT_SOURCE_DIR}/../common.cmake)

set(header_files
        custom_char.h
        lcd_tb.h
        myutils.h)

set(source_files
        lcd_tb.c)

add_library(lcdlib ${header_files} ${source_files})
