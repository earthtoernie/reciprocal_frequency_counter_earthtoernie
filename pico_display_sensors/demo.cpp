
#include <sstream>
#include <iomanip>
#include "hardware/i2c.h"


#include "pico_display.hpp"

#define PICO_DEFAULT_I2C 1
#define PICO_DEFAULT_I2C_SDA_PIN 26
#define PICO_DEFAULT_I2C_SCL_PIN 27

using namespace pimoroni;

static const uint8_t SHT_40_ADDRESS = 0x44;

// minicom -D /dev/ttyACM0 -b 115200

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay pico_display(buffer);

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

//void displayA(){
//    // set the colour of the pen
//    // parameters are red, green, blue all between 0 and 255
//    pico_display.set_pen(30, 40, 50);
//
//    // fill the screen with the current pen colour
//    pico_display.clear();
//
//    // draw a box to put some text in
//    pico_display.set_pen(10, 20, 30);
//    Rect text_rect(10, 10, 150, 150);
//    pico_display.rectangle(text_rect);
//
//    // write some text inside the box with 10 pixels of margin
//    // automatically word wrapping
//    text_rect.deflate(10);
//    pico_display.set_pen(110, 120, 130);
//    pico_display.text("This is a message", Point(text_rect.x, text_rect.y), text_rect.w);
//
//    // now we've done our drawing let's update the screen
//    pico_display.update();
//}

std::string leftExtend(std::string s, int totalLen) {
    int startLen = s.size();
    int lenDiff = totalLen - startLen;
    std::string toReturn = "";

    while(lenDiff>0){
        toReturn += "_";
        lenDiff--;
    }
    return toReturn + s;
}

void display(float tempC, float humidityPct, float mA) {
    float tempF = (tempC * 1.8) + 32;
    float maPct = (mA - 4.0) / 16.0;
    std::stringstream tempFStream;
    std::stringstream humidityPctFstream;
    std::stringstream mAFStream;
    std::stringstream mAStream;
    std::stringstream mAPctStream;

    tempFStream << std::fixed << std::setprecision(2) << tempF << "F";
    humidityPctFstream << std::fixed << std::setprecision(2) << humidityPct << "%";
    mAStream << std::fixed << std::setprecision(8) << mA << "%";
    mAPctStream << std::fixed << std::setprecision(8) << maPct << "%";

    pico_display.set_pen(255, 40, 50);
    pico_display.text(leftExtend(humidityPctFstream.str(), 7), Point(0, 0), 100, 5);
    pico_display.set_pen(0, 0, 255);
    pico_display.text(leftExtend(tempFStream.str(), 7), Point(0, 30), 100, 5);
    pico_display.set_pen(0, 255, 255);

    pico_display.text("_4mA", Point(0, 60), 100, 5);
    pico_display.text("__9%", Point(120, 60), 200, 5);


//    pico_display.set_pen(255, 40, 50);
//    pico_display.text("_23.45%", Point(0, 0), 100, 5);
//    pico_display.set_pen(0, 0, 255);
//    pico_display.text("_00.00F", Point(0, 30), 100, 5);
//    pico_display.set_pen(0, 255, 255);
//
//    pico_display.text("_4mA", Point(0, 60), 100, 5);
//    pico_display.text("__9%", Point(120, 60), 200, 5);


    pico_display.update();
}

void readSht(float tempHumidity[]){
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
    tempHumidity[0] = t_degC_float;
    tempHumidity[1] = rh_pRH;

}


int main() {
    stdio_init_all();
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
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
    // Make the I2C pins available to picotool
//    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    pico_display.init();
    pico_display.update();

    // set the backlight to a value between 0 and 255
    // the backlight is driven via PWM and is gamma corrected by our
    // library to give a gorgeous linear brightness range.
    pico_display.set_backlight(100);
    pico_display.clear();

    while(true) {
        pico_display.clear();
        sleep_ms(1000);
        float tempHumidity[2];
        readSht(tempHumidity);
        display(tempHumidity[0], tempHumidity[1], 2);
        // detect if the A button is pressed (could be A, B, X, or Y)
        if(pico_display.is_pressed(pico_display.Y)) {
            // make the led glow green
            // parameters are red, green, blue all between 0 and 255
            // these are also gamma corrected
            //pico_display.set_led(0, 255, 0);
            display(2, 3, 2);
        }


    }
}

