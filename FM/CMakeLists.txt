#Set the mimum required version of CMake
cmake_minimum_required(VERSION 3.12)

#Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

#Set the project name, the programming language
project(E2E C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#Initialize the SDK
pico_sdk_init()

add_subdirectory(FatFs_SPI build)

#Add an executable target to be build from the source files
add_executable(E2E 
            E2E.cpp
            GT_902PMGG_irq.cpp
            BME280.cpp
            hw_config.c
            )

#Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(E2E pico_stdlib hardware_gpio hardware_i2c hardware_pwm hardware_uart hardware_timer pico_binary_info FatFs_SPI)

#Enable USB output, disable UART output
pico_enable_stdio_usb(E2E 1)
pico_enable_stdio_uart(E2E 0)

#Create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(E2E)