macro(add_library_directory name)
    include_directories(${name})
    add_subdirectory(${name})
endmacro(add_library_directory)

macro(add_library_dependency target name)
    add_dependencies(${target} ${name})
    target_link_libraries(${target} ${name})
endmacro(add_library_dependency)

macro(flash name baud port)
    add_custom_command(
            TARGET ${name} POST_BUILD
            COMMAND avr-strip ${name}
            COMMAND avr-size --format=avr --mcu=atmega64 ${name}
            COMMAND avr-objcopy -O ihex -R.eeprom ${name} ${name}.hex
            COMMAND c45b -d -b ${baud} -p ${port} -f ${name}.hex -r
    )
endmacro(flash)
