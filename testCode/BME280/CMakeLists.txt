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

#Add an executable target to be build from the source files
add_executable(bme280 bme280.cpp example.cpp)

#Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(bme280 pico_stdlib hardware_i2c pico_binary_info)

#Enable USB output, disable UART output
pico_enable_stdio_usb(bme280 1)
pico_enable_stdio_uart(bme280 0)

#Create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(bme280)