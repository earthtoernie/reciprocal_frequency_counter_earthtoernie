add_executable(sht40
        sht40.cpp
        )

# pull in common dependencies
target_link_libraries(sht40 pico_stdlib hardware_i2c)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(sht40)

# add url via pico_set_program_url
example_auto_set_url(sht40)
