/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#define PICO_DEFAULT_I2C 1
#define PICO_DEFAULT_I2C_SDA_PIN 26
#define PICO_DEFAULT_I2C_SCL_PIN 27


#include <stdio.h>
#include <pico/binary_info/code.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// mcp9808_i2c.c
//

static const uint8_t SHT_40_ADDRESS = 0x44;

int reg_write(i2c_inst_t *i2c,
              const uint addr,
              const uint8_t reg,
              uint8_t *buf,
              const uint8_t nbytes);

int reg_read(i2c_inst_t *i2c,
             const uint addr,
             const uint8_t reg,
             uint8_t *buf,
             const uint8_t nbytes);

// Write 1 byte to the specified register
int reg_write(i2c_inst_t *i2c,
              const uint addr,
              const uint8_t reg,
              uint8_t *buf,
              const uint8_t nbytes) {
    int num_bytes_read = 0;
    uint8_t msg[nbytes + 1];

    // Check to make sure caller is sending 1 or more bytes
    if (nbytes < 1) {
        return 0;
    }

    // Append register address to front of data packet
    msg[0] = reg;
    for (int i = 0; i < nbytes; i++) {
        msg[i + 1] = buf[i];
    }

    // Write data to register(s) over I2C
    i2c_write_blocking(i2c, addr, msg, (nbytes + 1), false);

    return num_bytes_read;
}
int main() {
    stdio_init_all();
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
//    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    uint8_t buf[6];

    uint8_t msg1[2];
    msg1[0] = 0x94;
    i2c_write_blocking(i2c_default, SHT_40_ADDRESS, msg1, 1, false);
    sleep_ms(10);

    uint8_t msg2[2];
    msg2[0] = 0xFD;
    i2c_write_blocking(i2c_default, SHT_40_ADDRESS, msg2, 1, false);
    sleep_ms(10);

    uint8_t msg3[6];
    i2c_read_blocking(i2c_default, SHT_40_ADDRESS, msg3, 6, false);

    while (true) {

        uint8_t msg1[2];
        msg1[0] = 0x94;
        i2c_write_blocking(i2c_default, SHT_40_ADDRESS, msg1, 1, false);
        sleep_ms(10);

        uint8_t msg2[2];
        msg2[0] = 0xFD;
        i2c_write_blocking(i2c_default, SHT_40_ADDRESS, msg2, 1, false);
        sleep_ms(10);

        uint8_t rx_bytes[6];
        i2c_read_blocking(i2c_default, SHT_40_ADDRESS, rx_bytes, 6, false);

        uint16_t t_ticks = rx_bytes[0] * 256 + rx_bytes[1];
        uint16_t rh_ticks = rx_bytes[3] * 256 + rx_bytes[4];
        uint8_t checksum_t = rx_bytes[2];
        uint8_t checksum_rh = rx_bytes[5];
        //uint16_t t_degC_uint = -45 + 175 * t_ticks/65535;
        float t_degC_float = -45 + 175 * (float)t_ticks/65535;
        float rh_pRH = -6 + 125 * (float)rh_ticks/65535;


        //printf("temp C: %u\n", (unsigned int)t_degC_uint);
        printf("temp C: %.2f\n", t_degC_float);
        printf("temp F: %.2f\n", (t_degC_float * (9.0/5.0))+32);
        printf("    rh: %.2f\n", rh_pRH);



        printf("Hello, sht40!\n");
        sleep_ms(500);
    }
    return 0;
}
