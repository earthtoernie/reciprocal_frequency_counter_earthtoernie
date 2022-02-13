#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"

#include "pico_pio_interrupts.pio.h"


#define PICO_R_LED_PIN 9
#define BTN_0 5
#define BTN_1 19
#define OUTPUT_SIG 10

PIO sd_pio = pio0;


static inline void pio_program_init(PIO pio, uint sm, uint offset) {

    //pio_gpio_init(pio, OUTPUT_SIG);

//    pio_sm_set_enabled(pio, )


//    pio_sm_set_consecutive_pindirs(pio, sm, OUTPUT_SIG, 1, true);
//    pio_sm_set_consecutive_pindirs(pio, sm, pin::ROW_6, 7, true);
//
//    pio_sm_config c = unicorn_program_get_default_config(offset);
//
//    // osr shifts right, autopull on, autopull threshold 8
//    sm_config_set_out_shift(&c, true, false, 32);
//
//    // configure out, set, and sideset pins
//    sm_config_set_out_pins(&c, pin::ROW_6, 7);
//    sm_config_set_sideset_pins(&c, pin::LED_CLOCK);
//    sm_config_set_set_pins(&c, pin::LED_DATA, 4);
//
//    // join fifos as only tx needed (gives 8 deep fifo instead of 4)
//    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
//
//    pio_sm_init(pio, sm, offset, &c);
//    pio_sm_set_enabled(pio, sm, true);
}

int main() {

//    gpio_init(OUTPUT_SIG);
//    gpio_set_dir(OUTPUT_SIG, GPIO_OUT);
//    gpio_put(OUTPUT_SIG, 1);

    int sm = 0;//pio_claim_unused_sm(sd_pio, true);
    uint offset_blinker_program = pio_add_program(sd_pio, &blinker_program);
    blinker_program_init(sd_pio, sm, offset_blinker_program, OUTPUT_SIG);

    //pio_program_init(sd_pio, sm, offset_blinker_program);
    pio_sm_set_enabled(sd_pio, sm, false);
    while (true){}
}