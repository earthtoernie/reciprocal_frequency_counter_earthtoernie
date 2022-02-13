#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"

#include "pico_pio_interrupts.pio.h"


#define PICO_R_LED_PIN 9
#define BTN_0 5
#define BTN_1 19
#define OUTPUT_SIG 10
#define WAIT_1 5
#define OUTPUT_SIDE 15
#define wait_2 19

PIO sd_pio = pio0;

int main() {

//    gpio_init(OUTPUT_SIG);
//    gpio_set_dir(OUTPUT_SIG, GPIO_OUT);
//    gpio_put(OUTPUT_SIG, 1);

    int sm_blocker = 0;//pio_claim_unused_sm(sd_pio, true);
    uint offset_blocker_program = pio_add_program(sd_pio, &blocker_program);
    blocker_program_init(sd_pio, sm_blocker, offset_blocker_program, WAIT_1, OUTPUT_SIDE);
    pio_sm_set_enabled(sd_pio, sm_blocker, true);


    int sm_blinker = 1;//pio_claim_unused_sm(sd_pio, true);
    uint offset_blinker_program = pio_add_program(sd_pio, &blinker_program);
    blinker_program_init(sd_pio, sm_blinker, offset_blinker_program, OUTPUT_SIG);

    //pio_program_init(sd_pio, sm, offset_blinker_program);
    pio_sm_set_enabled(sd_pio, sm_blinker, true);
    while (true){}
}