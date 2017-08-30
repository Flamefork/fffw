set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER /usr/local/bin/avr-gcc)
set(CMAKE_CXX_COMPILER /usr/local/bin/avr-g++)

set(CSTANDARD "-std=gnu99")
set(CDEBUG "-gstabs")
set(CWARN "-Wall")
set(CTUNING "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums")
set(COPT "-Os")
set(CMCU "-mmcu=atmega64")
set(CDEFS "-DF_CPU=8000000UL -DTB_11P_DEVICE")

set(CFLAGS "${CMCU} ${CDEBUG} ${CDEFS} ${CINCS} ${COPT} ${CWARN} ${CSTANDARD} ${CEXTRA}")
set(CXXFLAGS "${CMCU} ${CDEFS} ${CINCS} ${COPT}")

set(CMAKE_C_FLAGS  ${CFLAGS})
set(CMAKE_CXX_FLAGS ${CXXFLAGS})

macro(add_project_library_dependency target name)
    include_directories(${PROJECT_SOURCE_DIR}/${name})
    add_subdirectory(${PROJECT_SOURCE_DIR}/${name})
    add_dependencies(${target} ${name})
    target_link_libraries(${target} ${name})
endmacro(add_project_library_dependency)

macro(create_hex name)
    add_custom_command(TARGET ${name} POST_BUILD COMMAND avr-objcopy ARGS -O ihex -R.eeprom ${name} ${name}.hex)
endmacro(create_hex)
