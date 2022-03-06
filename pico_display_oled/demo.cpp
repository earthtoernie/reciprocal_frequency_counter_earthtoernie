#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <hardware/regs/intctrl.h>
#include <hardware/irq.h>
#include <cstdio>
#include <pico/stdio.h>

#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"

#include "count.pio.h"

#include "pico/stdlib.h"

#include "ss_oled.h"

#include "ss_oled.hpp"



#define INPUT_PIN 5
#define GATE_PIN 4
#define PULSE_FIN_PIN 3

// i2c stuff. oled stuff
#define PICO_DEFAULT_I2C 1
#define PICO_DEFAULT_I2C_SDA_PIN 26
#define PICO_DEFAULT_I2C_SCL_PIN 27
#define PICO_I2C i2c_default
#define I2C_SPEED 100 * 1000

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

uint32_t max_count = 4294967295;// const((1 << 32) - 1), highest unsigned int


PIO sd_pio = pio0;
volatile bool update_flag = false;
uint32_t data [2] = { 0, 0 };

//https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__irq.html
void isr()
{
    pio_interrupt_clear(pio0, 0);
    //irq_clear(PIO0_IRQ_0);

    printf("IRQ: ");

    if(!update_flag){
        pio_sm_put(sd_pio, 0, 125000000);
        pio_sm_exec(sd_pio, 0, pio_encode_pull(false, false));
        //printf("irq: ");

        data[0] = pio_sm_get(sd_pio, 1);
        data[1] = pio_sm_get(sd_pio, 2);
        update_flag = true;
    }
//    pio_interrupt_clear(pio0, 0);
//    pio_interrupt_clear(pio0, 0);
//    irq_clear(PIO0_IRQ_0);
}

void init_sm(uint32_t freq) {
    gpio_init(INPUT_PIN);
    gpio_init(GATE_PIN);
    gpio_init(PULSE_FIN_PIN);
    gpio_set_dir(INPUT_PIN, GPIO_IN);
    gpio_set_dir(GATE_PIN, GPIO_OUT);
    gpio_set_dir(PULSE_FIN_PIN, GPIO_OUT);
    gpio_put(GATE_PIN, 1);
    gpio_put(PULSE_FIN_PIN, 1);


    // starts state machines.
    uint offset_gate_program = pio_add_program(sd_pio, &gate_program);
    uint offset_clock_count_program = pio_add_program(sd_pio, &clock_count_program);
    uint offset_pulse_count_program = pio_add_program(sd_pio, &pulse_count_program);

    gate_program_init(sd_pio, 0, offset_gate_program);
    pio_sm_put_blocking(sd_pio, 0, freq);
    pio_sm_exec(sd_pio, 0, pio_encode_pull(false, false));

    clock_count_program_init(sd_pio, 1, offset_clock_count_program);
    pio_sm_put(sd_pio, 1, max_count);
    pio_sm_exec(sd_pio, 1, pio_encode_pull(false, false));
//
    pulse_count_program_init(sd_pio, 2, offset_pulse_count_program);
    pio_sm_put(sd_pio, 2, max_count-1);
    pio_sm_exec(sd_pio, 2, pio_encode_pull(false, false));

//    pio_sm_set_enabled(sd_pio, 1, true);
//    pio_sm_set_enabled(sd_pio, 2, true);

    irq_set_exclusive_handler(PIO0_IRQ_0, isr);
    irq_set_enabled(PIO0_IRQ_0, true);
    pio_set_irq0_source_enabled(sd_pio, pis_interrupt0, true);

    pio_sm_set_enabled(sd_pio, 1, true);
    pio_sm_set_enabled(sd_pio, 2, true);
    pio_sm_set_enabled(sd_pio, 0, true);


}



//void count_handler(sm)
int main(){
    // minicom -D /dev/ttyACM0 -b 115200
    // https://forums.raspberrypi.com/viewtopic.php?t=316677
    stdio_init_all();
    printf("hello world!\n");
    static uint8_t  ubBuffer[1024];
    int i, j, rc;
    char szTemp[32];
    printf("a\n");
    picoSSOLED myOled(OLED_128x64, 0x3c, 0, 0, PICO_I2C, PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, I2C_SPEED);
    printf("b\n");
    rc = myOled.init() ;
    printf("c\n");


    while(1){
        printf("hello there!\n");

        sleep_ms(2000);
        if(rc != OLED_NOT_FOUND) {
            printf("oled found!\n");
        } else {
            printf("oled not found\n");
        }

    }


}



