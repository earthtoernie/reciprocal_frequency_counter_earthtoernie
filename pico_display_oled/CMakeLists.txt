add_executable(
        display_oled
        main.cpp
        image_data.cpp
)

target_sources(display_oled PRIVATE
        stripped.hpp
        ss_oled.cpp
        ss_oled.c
        ss_oled.h
        BitBang_I2C.h
        BitBang_I2C.c
        )

# Pull in pico libraries that we need
target_link_libraries(display_oled pico_stdlib hardware_i2c pico_multicore hardware_clocks hardware_pwm hardware_pio hardware_dma)

# create map/bin/hex file etc.
pico_add_extra_outputs(display_oled)

# enable usb output, disable uart output
pico_enable_stdio_usb(display_oled 0)
pico_enable_stdio_uart(display_oled 1)

# Create C header file with the name <pio program>.pio.h
pico_generate_pio_header(display_oled ${CMAKE_CURRENT_LIST_DIR}/count.pio)

#add_custom_command(OUTPUT ${CMAKE_CURRENT_LIST_DIR}/generated/pico_pio_interrupts.pio.h
#        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/pico_pio_interrupts.pio
#        COMMAND Pioasm ${CMAKE_CURRENT_LIST_DIR}/pico_pio_interrupts.pio ${CMAKE_CURRENT_LIST_DIR}/generated/pico_pio_interrupts.pio.h
#        )

# add url via pico_set_program_url
example_auto_set_url(display_oled)