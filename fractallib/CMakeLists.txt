CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

INCLUDE(${FFFW_BASE_DIRECTORY}/fffw_crosscompile.cmake)

PROJECT(fractallib)

MESSAGE(STATUS "\nCreating fractallib library...")

SET(HEADER_FILES
        axefx.h)

SET(SOURCE_FILES
        axefx.c)

ADD_LIBRARY(fractallib ${HEADER_FILES} ${SOURCE_FILES})
SET(FRACTAL_LIBRARY fractallib CACHE PATH "LCD library valiavle")
