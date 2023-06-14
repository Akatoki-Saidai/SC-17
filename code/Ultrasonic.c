#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define LOW 0
#define HIGH 1

#define Trig_PIN 2
#define Echo_PIN 3


int main() {

    stdio_init_all();
    gpio_init(Trig_PIN);
    gpio_init(Echo_PIN);
    gpio_set_dir(Trig_PIN, GPIO_OUT);
    gpio_set_dir(Echo_PIN, GPIO_IN);
    int timeout = 26100;
    double distance;
    absolute_time_t startTime, endTime;

    printf("Ultrasonic Sencer Start\n");

    while (true) {
        uint64_t count = 0;
        gpio_put(Trig_PIN, HIGH);
        sleep_us(10);
        gpio_put(Trig_PIN, LOW);
        while(gpio_get(Echo_PIN) == LOW){
            tight_loop_contents();
        }
        startTime = get_absolute_time();
        while(gpio_get(Echo_PIN) == HIGH){
            count++;
            sleep_us(1);
            if(count > timeout){
                break;
            }
        }
        endTime = get_absolute_time();
        distance = absolute_time_diff_us(startTime, endTime) / 2.0 * 340 * 100 / 1000000;
        printf("distance:%lfcm\n", distance);
        sleep_ms(1000);
    }
    return 0;
}