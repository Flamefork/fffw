# Flamefork firmware for BJ Devices TB-11P aimed for Fractal Axe-Fx II

### Modify

Check out `src/settings.h` for actual settings and `src/common_defs.h` for constants.

### Build

1. Install [CMake](https://cmake.org)
2. Prepare project with `mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" .. && cd ..`
3. Run `cmake --build build`

### Flash

1. Connect the controller to USB port
2. Install `c45b` bootloader frontend
3. Uncomment the `flash(...)` line in `CMakeLists.txt`
4. Update device name with your USB serial port connected to the controller
5. Build the project

### Read logs

Use `screen /dev/cu.usbserial-AI1WX4XP 19200` but use your USB serial port
instead of `cu.usbserial-AI1WX4XP`. `Ctrl+a+\` to exit, just in case.

## Credits

S. Burenkov for [awesome controllers](http://bjdevices.com/) and 
[open source library for them](https://github.com/BurenkovS/bjdevlib).
