CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

INCLUDE(${FFFW_BASE_DIRECTORY}/fffw_crosscompile.cmake)

PROJECT(lcdlib)

MESSAGE(STATUS "\nCreating lcdlib library...")

SET(HEADER_FILES
        custom_char.h
        lcd_tb.h
        myutils.h)

SET(SOURCE_FILES
        lcd_tb.c)

ADD_LIBRARY(lcdlib ${HEADER_FILES} ${SOURCE_FILES})
SET(LCD_LIBRARY lcdlib CACHE PATH "LCD library valiavle")
