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

uint32_t max_count = 4294967295;// const((1 << 32) - 1), highest unsigned int


PIO sd_pio = pio1;
bool update_flag = false;
uint16_t data [2] = { 0, 0 };

void init_sm(uint16_t freq, uint8_t input_pin, uint8_t gate_pin, uint8_t pulse_fin_pin) {
    // starts state machines.
    uint offset_gate_program = pio_add_program(sd_pio, &gate_program);
    uint offset_clock_count_program = pio_add_program(sd_pio, &clock_count_program);


    gate_program_init(sd_pio, 0, offset_gate_program, 0);
    pio_sm_put(sd_pio, 0, freq);
    pio_sm_exec(sd_pio, 0, pio_encode_pull(false, false));

    clock_count_program_init(sd_pio, 1, offset_clock_count_program, 0);
    pio_sm_put(sd_pio, 1, max_count);
    pio_sm_exec(sd_pio, 1, pio_encode_pull(false, false));

    pulse_count_program_init(sd_pio, 2, offset_clock_count_program, 0);
    pio_sm_put(sd_pio, 2, max_count-1);
    pio_sm_exec(sd_pio, 2, pio_encode_pull(false, false));

}

//https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__irq.html
void isr()
{

    printf("IRQ\n");
    if(!update_flag){
        pio_sm_put(sd_pio, 0, 12500);
        pio_sm_exec(sd_pio, 0, pio_encode_pull(false, false));
        data[0] = pio_sm_get(sd_pio, 1);
        data[1] = pio_sm_get(sd_pio, 2);
        update_flag = true;
    }
    pio_interrupt_clear(pio0, 0);
//    pio_interrupt_clear(pio0, 0);
//    irq_clear(PIO0_IRQ_0);
}

//void count_handler(sm)
int main(){
    stdio_init_all();


    printf("hello world\n");

    init_sm(12500000, 5, 4, 3);
    irq_set_exclusive_handler(PIO0_IRQ_0, isr);
    irq_set_enabled(PIO0_IRQ_0, true);
    pio_set_irq0_source_enabled(sd_pio, pis_interrupt0, true);

    uint32_t i;
    while(true){
        uint32_t clock_count = 2*(max_count - data[0]+1);
        uint32_t pulse_count = max_count - data[1];
        float frequency = pulse_count * (125000208.6 / clock_count);
        printf("%d\n", i);
        printf("Clock count: %u\n", clock_count);
        printf("Input count: %u\n", pulse_count);
        printf("Frequency: %f", frequency);
    }



    //pio_sm_set_wrap()

}



