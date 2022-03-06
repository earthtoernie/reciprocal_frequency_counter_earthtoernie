#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <hardware/regs/intctrl.h>
#include <hardware/irq.h>
#include <cstdio>
#include <pico/stdio.h>
#include <string>

#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "pico/multicore.h"

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

void demoLoop(picoSSOLED myOled, int rc){
    static uint8_t ucBuffer[1024];

    if (rc != OLED_NOT_FOUND)
    {
        myOled.fill(0,1);
        myOled.set_contrast(127);
        myOled.write_string(0,0,0,(char *)"**************** ", FONT_8x8, 0, 1);
        myOled.write_string(0,4,1,(char *)"Pi Pico SS_OLED", FONT_8x8, 0, 1);
        myOled.write_string(0,8,2,(char *)"running on the", FONT_8x8, 0, 1);
        myOled.write_string(0,8,3,(char *)"SSD1306 128x64", FONT_8x8, 0, 1);
        myOled.write_string(0,4,4,(char *)"monochrome OLED", FONT_8x8, 0, 1);
        myOled.write_string(0,0,5,(char *)"Written by L BANK", FONT_8x8, 0, 1);
        myOled.write_string(0,4,6,(char *)"Pico by M KOOIJ", FONT_8x8, 0, 1);
        myOled.write_string(0,0,7,(char *)"**************** ", FONT_8x8, 0, 1);
        sleep_ms(5000);
    }

    myOled.fill(0,1);
    myOled.write_string(0,0,0,(char *)"Now with 5 font sizes", FONT_6x8, 0, 1);
    myOled.write_string(0,0,1,(char *)"6x8 8x8 16x16", FONT_8x8, 0, 1);
    myOled.write_string(0,0,2,(char *)"16x32 and a new", FONT_8x8, 0, 1);
    myOled.write_string(0,0,3,(char *)"Stretched", FONT_12x16, 0, 1);
    myOled.write_string(0,0,5,(char *)"from 6x8", FONT_12x16, 0, 1);
    sleep_ms(5000);


    int x, y;
    myOled.fill(0, 1);
    myOled.write_string(0,0,0,(char *)"Backbuffer Test", FONT_NORMAL,0,1);
    myOled.write_string(0,0,1,(char *)"96 lines", FONT_NORMAL,0,1);
    sleep_ms(2000);
    for (x=0; x<OLED_WIDTH-1; x+=2)
    {
        myOled.draw_line(x, 0, OLED_WIDTH-x, OLED_HEIGHT-1, 1);
    };
    for (y=0; y<OLED_HEIGHT-1; y+=2)
    {
        myOled.draw_line(OLED_WIDTH-1,y, 0,OLED_HEIGHT-1-y, 1);
    };
    myOled.write_string(0,0,1,(char *)"Without backbuffer", FONT_SMALL,0,1);
    sleep_ms(2000);
    myOled.fill(0,1);
    for (x=0; x<OLED_WIDTH-1; x+=2)
    {
        myOled.draw_line(x, 0, OLED_WIDTH-1-x, OLED_HEIGHT-1, 0);
    }
    for (y=0; y<OLED_HEIGHT-1; y+=2)
    {
        myOled.draw_line(OLED_WIDTH-1,y, 0,OLED_HEIGHT-1-y, 0);
    }
    myOled.dump_buffer(ucBuffer);
    myOled.write_string(0,0,1,(char *)"With backbuffer", FONT_SMALL,0,1);
    sleep_ms(2000);

}
void initOledDisplay(picoSSOLED myOled){
//    IRQ: 8
//    Clock count: 125124944
//    Input count: 1001
//    Frequency: 1000.002136
    myOled.fill(0,1);
    myOled.write_string(0,0,0,(char *)"IRQ: **********", FONT_8x8, 0, 1);
    myOled.write_string(0,0,1,(char *)"Clock Count:", FONT_6x8, 0, 1);
    myOled.write_string(0,0,2,(char *)"**********", FONT_8x8, 0, 1);
    myOled.write_string(0,0,3,(char *)"Input Count:", FONT_6x8, 0, 1);
    myOled.write_string(0,0,4,(char *)"**********", FONT_8x8, 0, 1);

    myOled.write_string(0,0,6,(char *)"***.******", FONT_8x8, 0, 1);
}

void updateOled(uint32_t irqCount, uint32_t clockCount, uint32_t pulseCount, picoSSOLED myOled) {
    float frequency = pulseCount * (125000208.6 / clockCount);

    std::string irqCountString = "IRQ: " + std::to_string(irqCount);
    std::string clockCountString = std::to_string(clockCount);
    std::string pulseCountString = std::to_string(pulseCount);
    std::string frequencyString = std::to_string(frequency);


    myOled.write_string(0,0,0,(char *)"IRQ:           ", FONT_8x8, 0, 1);
    myOled.write_string(0,0,0,(char *)irqCountString.c_str(), FONT_8x8, 0, 1);
    myOled.write_string(0,0,2,(char *)clockCountString.c_str(), FONT_8x8, 0, 1);
    myOled.write_string(0,0,4,(char *)"          ", FONT_8x8, 0, 1);
    myOled.write_string(0,0,4,(char *)pulseCountString.c_str(), FONT_8x8, 0, 1);
    myOled.write_string(0,0,6,(char *)"            ", FONT_8x8, 0, 1);
    myOled.write_string(0,0,6,(char *)frequencyString.c_str(), FONT_8x8, 0, 1);


//    myOled.write_string(0,0,0,(char *)"IRQ: **********", FONT_8x8, 0, 1);
//    myOled.write_string(0,0,1,(char *)"Clock Count:", FONT_6x8, 0, 1);
//    myOled.write_string(0,0,2,(char *)"**********", FONT_8x8, 0, 1);
//    myOled.write_string(0,0,3,(char *)"Input Count:", FONT_6x8, 0, 1);
//    myOled.write_string(0,0,4,(char *)"**********", FONT_8x8, 0, 1);
//
//    myOled.write_string(0,0,6,(char *)"***.******", FONT_8x8, 0, 1);


}

void core1_interrupt_handler(){
    while (multicore_fifo_rvalid()){
        uint16_t raw = multicore_fifo_pop_blocking();

    }
    multicore_fifo_clear_irq();
}

void core1_entry(){
    // Configure core 1 interrupt
    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_interrupt_handler);
    irq_set_enabled(SIO_IRQ_PROC1, true);

}

//void count_handler(sm)
int main() {
    // minicom -D /dev/ttyACM0 -b 115200
    // https://forums.raspberrypi.com/viewtopic.php?t=316677
    stdio_init_all();
    printf("hello world!\n");
    int rc;
    picoSSOLED myOled(OLED_128x64, 0x3c, 0, 0, PICO_I2C, PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, I2C_SPEED);
    rc = myOled.init();
    // pio stuff
    init_sm(125000000);
    int i = 0;

//    multicore_launch_core1(core1_entry);
//    multicore_fifo_push_blocking(0);
    initOledDisplay(myOled);
    updateOled(i, 1234567898, 42, myOled);
//    demoLoop(myOled, rc);


    while (true) {
        if (update_flag == true) {
            uint32_t clock_count = 2 * (max_count - data[0] + 1);
            uint32_t pulse_count = max_count - data[1];
            float frequency = pulse_count * (125000208.6 / clock_count);
            printf("%d\n", i);
            printf("Clock count: %u\n", clock_count);
            printf("Input count: %u\n", pulse_count);
            printf("Frequency: %f\n", frequency);
            updateOled(i, clock_count, pulse_count, myOled);

            i++;
            update_flag = false;
        }


    }

}


//IRQ: 8
//Clock count: 125124944
//Input count: 1001
//Frequency: 1000.002136