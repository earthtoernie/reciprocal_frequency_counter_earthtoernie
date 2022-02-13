add_executable(
        pico_pio_interrupts
        demo.cpp
)

# Pull in pico libraries that we need
target_link_libraries(pico_pio_interrupts pico_stdlib hardware_pio)

# create map/bin/hex file etc.
pico_add_extra_outputs(pico_pio_interrupts)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_pio_interrupts 0)
pico_enable_stdio_uart(pico_pio_interrupts 1)

# Create C header file with the name <pio program>.pio.h
pico_generate_pio_header(pico_pio_interrupts ${CMAKE_CURRENT_LIST_DIR}/pico_pio_interrupts.pio)

#add_custom_command(OUTPUT ${CMAKE_CURRENT_LIST_DIR}/generated/pico_pio_interrupts.pio.h
#        DEPENDS ${CMAKE_CURRENT_LIST_DIR}/pico_pio_interrupts.pio
#        COMMAND Pioasm ${CMAKE_CURRENT_LIST_DIR}/pico_pio_interrupts.pio ${CMAKE_CURRENT_LIST_DIR}/generated/pico_pio_interrupts.pio.h
#        )

# add url via pico_set_program_url
example_auto_set_url(pico_pio_interrupts)