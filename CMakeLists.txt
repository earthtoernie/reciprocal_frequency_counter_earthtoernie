cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

# START adds in the pimoroni stuff
function(add_resource target file)
    get_filename_component(NAME ${ARGV1} NAME_WE)
    set(FILENAME ${ARGV1})

    add_custom_command(
            OUTPUT ${NAME}.o

            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}
            ${CMAKE_CURRENT_BINARY_DIR}

            COMMAND arm-none-eabi-ld -r -b binary -o ${NAME}.o ${FILENAME}
            DEPENDS ${FILENAME}
    )

    target_sources(${ARGV0} PRIVATE ${NAME}.o)
endfunction(add_resource)

include_directories(
        ${CMAKE_CURRENT_LIST_DIR}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-error")

add_subdirectory(common)
add_subdirectory(drivers)
add_subdirectory(libraries)
# END adds in the pimoroni stuff

include(example_auto_set_url.cmake)
# Add blink example
add_subdirectory(blink)

# Add hello world example
add_subdirectory(hello_world)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        #-Wno-error=format-truncation
        )

# Hardware-specific examples in subdirectories:
add_subdirectory(pico_display)
#add_subdirectory(sht40) custom
#add_subdirectory(adc)
#add_subdirectory(clocks)
#add_subdirectory(cmake)
#add_subdirectory(divider)
#add_subdirectory(dma)
#add_subdirectory(flash)
#add_subdirectory(gpio)
add_subdirectory(i2c)
#add_subdirectory(interp)
#add_subdirectory(multicore)
#add_subdirectory(picoboard)
add_subdirectory(pio)
#add_subdirectory(pwm)
#add_subdirectory(reset)
#add_subdirectory(rtc)
#add_subdirectory(spi)
#add_subdirectory(system)
#add_subdirectory(timer)
#add_subdirectory(uart)
#add_subdirectory(usb)
#add_subdirectory(watchdog)
