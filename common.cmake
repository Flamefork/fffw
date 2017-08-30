set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_COMPILER /usr/local/bin/avr-gcc)

set(CMAKE_C_FLAGS "\
    -std=gnu99 \
    -Os \
    -Wall \
    -mmcu=atmega64 \
    -DF_CPU=8000000UL \
    -DTB_11P_DEVICE \
")
