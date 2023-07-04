#Set the mimum required version of CMake
cmake_minimum_required(VERSION 3.12)

#Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

#Set the project name, the programming language
project(sc17 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#Initialize the SDK
pico_sdk_init()

#Add an executable target to be build from the source files
add_executable(GT_902PMGG GT_902PMGG_irq.cpp example.cpp)

#Pull in our pico_stdlib which aggregates commonly used features
target_link_libraries(GT_902PMGG pico_stdlib hardware_uart pico_binary_info hardware_irq)

#Enable USB output, disable UART output
pico_enable_stdio_usb(GT_902PMGG 1)
pico_enable_stdio_uart(GT_902PMGG 0)

#Create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(GT_902PMGG)