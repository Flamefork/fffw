macro(add_library_directory name)
    include_directories(${name})
    add_subdirectory(${name})
endmacro(add_library_directory)

macro(add_library_dependency target name)
    add_dependencies(${target} ${name})
    target_link_libraries(${target} ${name})
endmacro(add_library_dependency)

macro(create_hex name)
    add_custom_command(TARGET ${name} POST_BUILD COMMAND avr-objcopy ARGS -O ihex -R.eeprom ${name} ${name}.hex)
endmacro(create_hex)

macro(flash_hex name device baud)
    add_custom_command(TARGET ${name} POST_BUILD COMMAND c45b -d -b ${baud} -p ${device} -f ${name}.hex -r)
endmacro(flash_hex)
