#include <stdio.h>
#include "pico/stdlib.h"

const uint LED_PIN = 25;

int main() {
    //gpio_set_function(16, GPIO_FUNC_UART); //TX
    //gpio_set_function(17, GPIO_FUNC_UART); //RX
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        printf("LED ONnnnnnj\n");
        gpio_put(LED_PIN, 1);
        sleep_ms(1000);

        printf("LED OFF\n");
        gpio_put(LED_PIN, 0);
        sleep_ms(1000);
    }
}