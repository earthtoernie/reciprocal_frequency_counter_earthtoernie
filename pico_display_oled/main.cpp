#include <cstring>
#include <hardware/irq.h>
#include <cstdio>
#include <string>
#include "pico/stdio.h"
#include "pico/multicore.h"
#include "hardware/pio.h"
#include "count.pio.h"
#include "ss_oled.hpp"

#define INPUT_PIN 5
#define GATE_PIN 4
#define PULSE_FIN_PIN 3

// Button pins
#define BUTTON_WINDOW_PIN_16 16
#define BUTTON_SOURCE_PIN_17 17

// output pins
#define LED_PIN 25
#define EXT50_EN_PIN 12

// i2c stuff. oled stuff
#define PICO_DEFAULT_I2C 1
#define PICO_DEFAULT_I2C_SDA_PIN 26
#define PICO_DEFAULT_I2C_SCL_PIN 27
#define PICO_I2C i2c_default
#define I2C_SPEED 400000

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

#define GATE_err 0
#define GATE_1s 1
#define GATE_10ms 16
#define GATE_20ms 17
#define GATE_100ms 18
#define GATE_500ms 19

// global refference to display
extern picoSSOLED myOled(OLED_128x64, 0x3c, 0, 0, PICO_I2C, PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, I2C_SPEED);


// debounce control
unsigned long time_window_state_debounce = to_ms_since_boot(get_absolute_time());
unsigned long time_source_state_debounce = to_ms_since_boot(get_absolute_time());

const int delayTime = 300; // Delay for every push button may vary

// glbal state ow windows in inputs
int button_window_state; // 1 represents the smallest window
bool button_source_state;

void source_state_changed_callback(uint gpio, uint32_t events)
{
    if ((to_ms_since_boot(get_absolute_time()) - time_source_state_debounce) > delayTime)
    {
        // Recommend to not to change the position of this line
        time_source_state_debounce = to_ms_since_boot(get_absolute_time());
        printf("********** SOURCE button pressed!\n");

        // Interrupt function lines
        button_source_state = !button_source_state;
        gpio_put(LED_PIN, button_source_state);
        gpio_put(EXT50_EN_PIN, button_source_state);
        if (button_source_state == true)
        {
            myOled.write_string(0, 0, 1, (char *)"50 Ohm, Clock Count:", FONT_6x8, 0, 1);
        }
        else
        {
            myOled.write_string(0, 0, 1, (char *)"Sensor, Clock Count:", FONT_6x8, 0, 1);
        }
    }
}

void window_state_changed_callback(uint gpio, uint32_t events)
{
    if ((to_ms_since_boot(get_absolute_time()) - time_window_state_debounce) > delayTime)
    {
        // Recommend to not to change the position of this line
        time_window_state_debounce = to_ms_since_boot(get_absolute_time());
        printf("********** WINDOW button pressed!\n");

        // Interrupt function lines
        // TODO
    }
}

uint32_t max_count = 4294967295;// const((1 << 32) - 1), highest unsigned int


PIO sd_pio = pio0;
volatile bool update_flag = false;
uint32_t countTo = 125000000; // 1M
int set_pin = 0;
uint32_t data [2] = { 0, 0 };



//https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__irq.html
void isr()
{
    pio_interrupt_clear(pio0, 0);
    //irq_clear(PIO0_IRQ_0);

    printf("IRQ: ");

    if(!update_flag){
        pio_sm_put(sd_pio, 0, countTo);
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

    irq_set_exclusive_handler(PIO0_IRQ_0, isr);
    irq_set_enabled(PIO0_IRQ_0, true);
    pio_set_irq0_source_enabled(sd_pio, pis_interrupt0, true);

    pio_sm_set_enabled(sd_pio, 1, true);
    pio_sm_set_enabled(sd_pio, 2, true);
    pio_sm_set_enabled(sd_pio, 0, true);

}

void initOledDisplay(picoSSOLED myOled){
//    IRQ: 8
//    Clock count: 125124944
//    Input count: 1001
//    Frequency: 1000.002136
    myOled.fill(0,1);
    myOled.write_string(0,0,0,(char *)"IRQ: **********", FONT_8x8, 0, 1);
    myOled.write_string(0,0,1,(char *)"50 Ohm, Clock Count:", FONT_6x8, 0, 1);
    myOled.write_string(0,0,2,(char *)"**********", FONT_8x8, 0, 1);
    if(set_pin == GATE_10ms) myOled.write_string(0,0,3,(char *)"Input Count: 10ms", FONT_6x8, 0, 1);
    if(set_pin == GATE_20ms) myOled.write_string(0,0,3,(char *)"Input Count: 20ms", FONT_6x8, 0, 1);
    if(set_pin == GATE_100ms) myOled.write_string(0,0,3,(char *)"Input Count: 100ms", FONT_6x8, 0, 1);
    if(set_pin == GATE_500ms) myOled.write_string(0,0,3,(char *)"Input Count: 500ms", FONT_6x8, 0, 1);
    if(set_pin == GATE_1s) myOled.write_string(0,0,3,(char *)"Input Count: 1s", FONT_6x8, 0, 1);
    if(set_pin == GATE_err) myOled.write_string(0,0,3,(char *)"Input Count: err", FONT_6x8, 0, 1);

    myOled.write_string(0,0,4,(char *)"**********", FONT_8x8, 0, 1);
    myOled.write_string(0,0,5,(char *)"10m 20m 100m 500m 1 ", FONT_6x8, 1, 1);
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

    myOled.write_string(0,0,2,(char *)"            ", FONT_8x8, 0, 1);
    myOled.write_string(0,0,2,(char *)clockCountString.c_str(), FONT_8x8, 0, 1);

    myOled.write_string(0,0,4,(char *)"            ", FONT_8x8, 0, 1);
    myOled.write_string(0,0,4,(char *)pulseCountString.c_str(), FONT_8x8, 0, 1);
    myOled.write_string(0,0,6,(char *)"            ", FONT_8x8, 0, 1);
    myOled.write_string(0,0,6,(char *)frequencyString.c_str(), FONT_8x8, 0, 1);
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
    // python3 ./usr/lib/python3/dist-packages/serial/tools/miniterm.py - 115200
    // ctrl ]
    // https://forums.raspberrypi.com/viewtopic.php?t=316677
    // python3 -m serial.tools.list_ports -v
    // https://forums.raspberrypi.com/viewtopic.php?t=306876

    gpio_pull_up(GATE_10ms);
    gpio_pull_up(GATE_20ms);
    gpio_pull_up(GATE_100ms);
    gpio_pull_up(GATE_500ms);
    gpio_init(GATE_10ms);
    gpio_init(GATE_20ms);
    gpio_init(GATE_100ms);
    gpio_init(GATE_500ms);
    gpio_set_dir(GATE_10ms, GPIO_IN);
    gpio_set_dir(GATE_20ms, GPIO_IN);
    gpio_set_dir(GATE_100ms, GPIO_IN);
    gpio_set_dir(GATE_500ms, GPIO_IN);

    gpio_pull_up(2);
    gpio_set_dir(2, GPIO_OUT);
    gpio_put(2, 1);

    bool gate_10ms_set = !gpio_get(GATE_10ms);
    bool gate_20ms_set = !gpio_get(GATE_20ms);
    bool gate_100ms_set = !gpio_get(GATE_100ms);
    bool gate_500ms_set = !gpio_get(GATE_500ms);
    stdio_init_all();

    button_source_state = true;

    // start init of buttons
    // just to know when the board is on ... not related to debounce
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, button_source_state);
    
    gpio_init(EXT50_EN_PIN);
    gpio_set_dir(EXT50_EN_PIN, GPIO_OUT);
    gpio_put(EXT50_EN_PIN, button_source_state);

    gpio_init(BUTTON_SOURCE_PIN_17);
    gpio_set_dir(BUTTON_SOURCE_PIN_17, GPIO_IN);
    gpio_pull_up(BUTTON_SOURCE_PIN_17);


    gpio_init(BUTTON_WINDOW_PIN_16);
    gpio_set_dir(BUTTON_WINDOW_PIN_16, GPIO_IN);
    gpio_pull_up(BUTTON_WINDOW_PIN_16);


    // interrupt
    // todo, "gpio_set_irq_enabled_with_callback" is broken
    // see https://forums.raspberrypi.com/viewtopic.php?t=339227&sid=64b002dadef60993ff6d878310481399
    // see https://github.com/raspberrypi/pico-sdk/releases
    // https://www.embedded.com/interrupts-in-c/ (NVIC)
    gpio_set_irq_enabled_with_callback(BUTTON_SOURCE_PIN_17, GPIO_IRQ_EDGE_FALL , true, &source_state_changed_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_WINDOW_PIN_16, GPIO_IRQ_EDGE_FALL , true, &window_state_changed_callback);


    // end init of buttons

    int total_set = 0;
    if(gate_10ms_set) {set_pin = GATE_10ms; total_set++; countTo = 1250000;}
    if(gate_20ms_set) {set_pin = GATE_20ms; total_set++; countTo = 2500000;}
    if(gate_100ms_set) {set_pin = GATE_100ms; total_set++; countTo = 12500000;}
    if(gate_500ms_set) {set_pin = GATE_500ms; total_set++; countTo = 62500000;}
    if(total_set > 1) {set_pin = GATE_err;}
    if(total_set == 0) {set_pin = GATE_1s;}



    printf("hello world!\n");
//    printf("%s %s %s %s \n" , gate_10ms_set?"true":"false", gate_20ms_set?"true":"false", gate_100ms_set?"true":"false", gate_500ms_set?"true":"false");
//    printf("set pin: %i \n", set_pin);

    int rc;
    //picoSSOLED myOled(OLED_128x64, 0x3c, 0, 0, PICO_I2C, PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, I2C_SPEED);
    rc = myOled.init();
    // pio stuff
    init_sm(125000000);
    uint32_t i = 0;

//    multicore_launch_core1(core1_entry);
//    multicore_fifo_push_blocking(0);
    initOledDisplay(myOled);
    //updateOled(i, 1234567898, 42, myOled);
//    demoLoop(myOled, rc);


    while (true) {
        if (update_flag == true) {
            uint32_t clock_count = 2 * (max_count - data[0] + 1);
            uint32_t pulse_count = max_count - data[1];
            float frequency = pulse_count * (125000208.6 / clock_count);
            printf("%u %u %u\n", i, data[0], data[1]);
//            printf("d_0: %u\n", data[0]);
//            printf("d_1: %u\n", data[1]);
            printf("Clock count: %u\n", clock_count);
            printf("Input count: %u\n", pulse_count);
            printf("Frequency: %f\n", frequency);
            updateOled(i, clock_count, pulse_count, myOled);

            i++;
            update_flag = false;
        }
    }

}