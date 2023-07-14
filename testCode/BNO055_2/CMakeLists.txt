cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(BNO055 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(BNO055
        BNO055.c
        )

# pull in common dependencies
target_link_libraries(BNO055 pico_stdlib hardware_gpio hardware_i2c)

# enable usb output, disable uart output
pico_enable_stdio_usb(BNO055 1)
pico_enable_stdio_uart(BNO055 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(BNO055)
