CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

INCLUDE(${FFFW_BASE_DIRECTORY}/fffw_crosscompile.cmake)

PROJECT(tblib)

MESSAGE(STATUS "\nCreating tbseries library...")

SET(HEADER_FILES
        adc.h
        bjdevlib_tb.h
        button.h
        crc8.h
        expression.h
        led.h
        lldled.h
        log.h
        midi.h
        onewire.h
        pedal_led.h
        pinout.h
        portio.h
        timer.h
        uart.h
        unique_id.h)

SET(SOURCE_FILES
        adc.c
        bjdevlib_tb.c
        button.c
        crc8.c
        expression.c
        led.c
        lldled.c
        midi.c
        onewire.c
        pedal_led.c
        portio.c
        timer.c
        uart.c
        unique_id.c)

ADD_LIBRARY(tblib ${HEADER_FILES} ${SOURCE_FILES})
SET(TB_LIBRARY tblib CACHE PATH "LCD library valiavle")
