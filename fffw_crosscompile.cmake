###############################################################################
# Copyright (c) 2011 Dmitry Suvorov <D.Suvorov90@gmail.com>
###############################################################################
# Cmake for cross compilation for AVR
################################################################################

SET(CMAKE_SYSTEM_NAME Generic)

SET(CMAKE_C_COMPILER /usr/local/bin/avr-gcc)
SET(CMAKE_CXX_COMPILER /usr/local/bin/avr-g++)

SET(CSTANDARD "-std=gnu99")
SET(CDEBUG "-gstabs")
SET(CWARN "-Wall -Wstrict-prototypes")
SET(CTUNING "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums")
SET(COPT "-Os")
SET(CMCU "-mmcu=atmega64")
SET(CDEFS "-DF_CPU=8000000UL -DTB_11P_DEVICE")

SET(CFLAGS "${CMCU} ${CDEBUG} ${CDEFS} ${CINCS} ${COPT} ${CWARN} ${CSTANDARD} ${CEXTRA}")
SET(CXXFLAGS "${CMCU} ${CDEFS} ${CINCS} ${COPT}")

SET(CMAKE_C_FLAGS  ${CFLAGS})
SET(CMAKE_CXX_FLAGS ${CXXFLAGS})
