cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(project1 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(TempSense
    TempSense.c
)

target_link_libraries(TempSense
    pico_stdlib
    hardware_i2c
)

pico_enable_stdio_usb(TempSense 1)
pico_enable_stdio_uart(TempSense 0)

pico_add_extra_outputs(TempSense)