add_executable(code
        Ultrasonic.c
        )

# pull in common dependencies
target_link_libraries(code pico_stdlib hardware_gpio hardware_timer)

# enable usb output, disable uart output
pico_enable_stdio_usb(code 1)
pico_enable_stdio_uart(code 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(code)