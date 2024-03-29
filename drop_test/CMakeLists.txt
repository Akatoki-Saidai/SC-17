#Set the mimum required version of CMake
cmake_minimum_required(VERSION 3.12)

#Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

#Set the project name, the programming language
project(SC-17 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#Initialize the SDK
pico_sdk_init()

add_subdirectory(FatFs_SPI build)

#Add an executable target to be build from the source files
add_executable(drop_test drop_test.cpp bme280.cpp hw_config.c)

#Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(drop_test pico_stdlib hardware_i2c pico_binary_info FatFs_SPI)

#Enable USB output, disable UART output
pico_enable_stdio_usb(drop_test 1)
pico_enable_stdio_uart(drop_test 0)

#Create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(drop_test)