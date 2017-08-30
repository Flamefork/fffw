cmake_minimum_required(VERSION 2.6.0)

include(${PROJECT_SOURCE_DIR}/common.cmake)

project(tblib)

set(header_files
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

set(source_files
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

add_library(tblib ${header_files} ${source_files})
