#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/timer.h"

#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

#define RX_PIN_1 8
#define TX_PIN_1 9

absolute_time_t startTime, endTime;
char *coordinate_char, *area_char;
bool isTextReceived;
int coordinate, area;

void uart1_init(){
    gpio_init(RX_PIN_1);
    gpio_init(TX_PIN_1);
    gpio_set_function(RX_PIN_1, GPIO_FUNC_UART);
    gpio_set_function(TX_PIN_1, GPIO_FUNC_UART);
    uart_init(uart1, BAUD_RATE);
    uart_set_format(uart1, DATA_BITS, STOP_BITS, PARITY);

}

void SendCoordinate(){
    char c;
    char str[100] = {0};
    bool isTextReceived = false;

    uart_puts(uart1, "SendCoordinate");
    printf("sent\n");
    startTime = get_absolute_time();
    while (!uart_is_readable(uart1)) {
        tight_loop_contents();
        endTime = get_absolute_time();
        if(absolute_time_diff_us(startTime, endTime) > 10000000){
            break;
        }
    }
    while (uart_is_readable(uart1)) {
        c = uart_getc(uart1);
        strncat(str, &c, 1); // 1文字ずつ追加する
        sleep_ms(10);
        isTextReceived = true;
    }
    if(isTextReceived){
        coordinate_char = strtok(str, ",");
        area_char = strtok(NULL, ",");
        printf("coordinate_char: %s, ", coordinate_char);
        printf("area_char: %s\n", area_char);
        coordinate = atoi(coordinate_char);
        area = atoi(area_char);
        printf("cooedinate_char: %d, ", coordinate);
        printf("area: %d\n", area);
    }else{
        printf("not received\n");
        //break;
    }
}