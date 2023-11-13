if (TARGET tinyusb_device)
    add_executable(squaregen
            main.c
            )

    # pull in common dependencies
    target_link_libraries(squaregen pico_stdlib)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(squaregen 0)
    pico_enable_stdio_uart(squaregen 1)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(squaregen)

    # add url via pico_set_program_url
    example_auto_set_url(squaregen)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building squaregen because TinyUSB submodule is not initialized in the SDK")
endif()

# target_compile_definitions(squaregen PRIVATE
#   PICO_DEFAULT_UART=0
#   PICO_DEFAULT_UART_TX_PIN=16
#   PICO_DEFAULT_UART_RX_PIN=17
# )