
add_executable(
        display_sensors
        demo.cpp
        image_data.cpp
)

# Pull in pico libraries that we need
target_link_libraries(display_sensors pico_stdlib hardware_spi hardware_clocks hardware_pwm hardware_pio hardware_dma pico_display)

# create map/bin/hex file etc.
pico_add_extra_outputs(display_sensors)

# enable usb output, disable uart output
pico_enable_stdio_usb(display_sensors 0)
pico_enable_stdio_uart(display_sensors 1)

# Create C header file with the name <pio program>.pio.h
#pico_generate_pio_header(display ${CMAKE_CURRENT_LIST_DIR}/count.pio)

#add_custom_command(OUTPUT ${CMAKE_CURRENT_LIST_DIR}/generated/count.pio.h
#        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/count.pio
#        COMMAND Pioasm ${CMAKE_CURRENT_LIST_DIR}/count.pio ${CMAKE_CURRENT_LIST_DIR}/generated/count.pio.h
#        )

# add url via pico_set_program_url
example_auto_set_url(display_sensors)