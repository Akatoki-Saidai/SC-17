#include "string.h"
#include "math.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/uart.h"

#include "bme280.hpp"

#include "BNO055.h"

#include "GT_902PMGG_irq.hpp"

#include "motor_dual.hpp"

#include "f_util.h"
#include "ff.h"
#include "rtc.h"
#include "hw_config.h"

#define PI 3.14159265359
#define RAD_TO_DEG(rad) ((rad) * (180.0 / PI))

int fase = 3; //スタートフェーズ　1:待機，2:落下，3:分離，4:遠距離，　5:近距離

double goal_lat = 35.85973464764691, goal_lon = 139.60499687714605; //ゴール座標

float calc_target_angle(GPS::Measurement_t gps, double heading);

float standerd_altitude;

#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

#define RX_PIN_1 8
#define TX_PIN_1 9


int main(){
    stdio_init_all();
    time_init();
    sleep_ms(1000);

    BME280 myBME280(
        1,          // I2C0 or I2C1
        14,          // SDA pin is GPIOx
        15,          // SCL pin is GPIOx
        0x76,       // SDO--GND:0x76 , SDO--VDD:0x77 (bme280のSDOピンをrasberryPIのどこに繋げるかでアドレスを選択)
        400 * 1000, // x Hz (500*1000 is 500 kHz)
        BME280::MODE::MODE_FORCED   // これは気にしなくてよい
    );
    BME280::Measurement_t bme;

    GPS myGPS(
        0,    //uart0 or uart1
        16,   //tx_pin(GPIO_no)  RP_pico:TX----RX:GPS(Green)
        17,   //rx_pin(GPIO_no)  RP_pico:RX----TX:GPS(Orange)
        9600  //baud_rate
    );
    GPS::Measurement_t gps;

    BNO055_init();
    motor_init();

    sleep_ms(100);

    myBME280.measure();
    myGPS.set_target(goal_lat, goal_lon);
    myGPS.measure();
    
    sleep_ms(1000);

    for(int i = 0; i < 10; i++){
        bme = myBME280.measure();
        standerd_altitude += bme.altitude_2;
        sleep_ms(10);
    }
    standerd_altitude /= 10;    

    gpio_init(RX_PIN_1);
    gpio_init(TX_PIN_1);
    gpio_set_function(RX_PIN_1, GPIO_FUNC_UART);
    gpio_set_function(TX_PIN_1, GPIO_FUNC_UART);
    uart_init(uart1, BAUD_RATE);
    uart_set_format(uart1, DATA_BITS, STOP_BITS, PARITY);

    sd_card_t *pSD = sd_get_by_num(0);
    FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
    if (FR_OK != fr) panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    FIL fil;
    const char* const filename = "CanSat_Logdata.txt";
    fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);

    f_printf(&fil, "latitude, longitude, distance, ");
    f_printf(&fil, "temperature, humidity, pressure, altitude, ");
    f_printf(&fil, "accel_X, accel_Y, accel_Z, resultant_accel");
    f_printf(&fil, "mag_X, mag_Y, mag_Z, ");
    f_printf(&fil, "azimuth, derection, target_angle, ");
    f_printf(&fil, "fase\n");
    fr = f_close(&fil);

    sleep_ms(1000);

    while(true){
        bme = myBME280.measure();
        gps = myGPS.measure();
        BNO055_accel_mag();
        float target_angle = calc_target_angle(gps, heading); 
 

        switch(fase){
            case 1:     //待機フェーズ
                if(resultant_accel > 20){
                    fase = 2;
                    sleep_ms(100);
                    break;
                }else{
                    break;
                }

            case 2:     //落下フェーズ
                if(fabs(bme.altitude_2 - standerd_altitude) < 10){
                    fase = 3;
                    sleep_ms(100);
                }else{
                    break;
                }
            
            case 3:     //分離フェーズ
                forward(1);
                sleep_ms(2000);
                forward(0.2);
                sleep_ms(500);
                forward(0);
                sleep_ms(1000);
                fase = 4;
                sleep_ms(100);
                break;
            
            case 4:     //遠距離フェーズ
                if(gps.target_distance < 5){
                    fase = 5;
                    sleep_ms(100);
                    break;
                }
                if(target_angle < 45 || target_angle > 315){
                    printf("forward\n");
                    forward(1);
                    sleep_ms(3000);
                    forward(0.2);
                    sleep_ms(100);
                    forward(0);
                    sleep_ms(100);
                    break;
                }else if((target_angle) <180){
                    printf("left\n");
                    left(1);
                    sleep_ms(500);
                    left(0);
                    sleep_ms(100);
                    break;
                }else{
                    printf("right\n");
                    right(1);
                    sleep_ms(500);
                    right(0);
                    sleep_ms(100);
                    break;
                }

            case 5:     //近距離フェーズ
                char *c;
                uart_puts(uart1, "s");
                sleep_ms(500);
                while (!uart_is_readable(uart1)) {
                    tight_loop_contents();
                }
                while (uart_is_readable(uart1)) {
                    *c = uart_getc(uart1);
                    putchar(*c);
                }
                if(*c == '0'){
                    printf("GOAL\n");
                    return 0;
                }else{
                    if(*c == '1'){
                        right(1);
                        sleep_ms(500);
                        right(0);
                        sleep_ms(100);
                        break;
                    }else if(*c == '2'){
                        forward(1);
                        sleep_ms(1000);
                        forward(0);
                        sleep_ms(100);
                        break;
                    }else if(*c == '3'){
                        left(1);
                        sleep_ms(500);
                        left(0);
                        sleep_ms(100);
                        break;
                    }else{
                        right(1);
                        sleep_ms(1000);
                        right(0);
                        sleep_ms(100);
                        break;
                    }
                }
        }

        //sleep_ms(100);
        printf("lat:%.10f, lon:%.10f, distance:%10.6f\n",gps.lat, gps.lon, gps.target_distance );
        sleep_ms(10);
        printf("temperature:%6.3f`C, humidity:%6.3f %c, pressure:%6.3fhPa, altitude:%6.3fm\n", bme.temperature, bme.humidity, bme.pressure, bme.altitude_2);
        printf("acceleration X:%6.2lf, Y:%6.2lf, Z: %6.2lf, resultant: %6.2lf, \n", f_accelX, f_accelY, f_accelZ, resultant_accel);
        printf("geomagnetism X: %6.2lf, Y: %6.2lf, Z: %6.2lf\n", f_magX, f_magY, f_magZ);
        printf("azimuth:%8.5f, derection:%.2lf, target_angle:%8.5f\n", gps.target_angle, heading, target_angle);
        printf("fase:%d\n", fase);
        
        sleep_ms(100);
        fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
        f_printf(&fil, "%.10f, %.10f, %10.6f, ",gps.lat, gps.lon, gps.target_distance );
        f_printf(&fil, "%6.3f, %6.3f, %6.3f, %6.3f, ", bme.temperature, bme.humidity, bme.pressure, bme.altitude_2);
        f_printf(&fil, "%6.2lf, %6.2lf, %6.2lf, %6.2lf", f_accelX, f_accelY, f_accelZ, resultant_accel);
        f_printf(&fil, "%6.2lf, %6.2lf, %6.2lf, ", f_magX, f_magY, f_magZ);
        f_printf(&fil, "%8.5f, %.2lf, %8.5f, ", gps.target_angle, heading, target_angle);
        f_printf(&fil, "%d\n", fase);
        fr = f_close(&fil);
    }
}

float calc_target_angle(GPS::Measurement_t gps_value, double heading){
    double goal_vector_x = goal_lon - gps_value.lon;
    double goal_vector_y = goal_lat - gps_value.lat;
    double magnetic_vector_x = cos(heading * PI / 180.0);
    double magnetic_vector_y = sin(heading * PI / 180.0);
    double vector_x = goal_vector_x + magnetic_vector_x;
    double vector_y = goal_vector_y + magnetic_vector_y;

    double direction_angle = atan2(vector_y, vector_x);

    if (direction_angle < 0) {
        direction_angle += 2 * PI;
    }

    double target_angle = RAD_TO_DEG(direction_angle);
    target_angle += 90;

    if(target_angle > 360){
        target_angle -= 360;
    }
    return target_angle;
}