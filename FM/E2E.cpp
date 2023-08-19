#include "stdio.h"
#include "string.h"
#include "math.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/uart.h"
#include "hardware/timer.h"

#include "BME280.hpp"

#include "BNO055.h"

#include "GT_902PMGG_irq.hpp"

#include "motor_dual.hpp"

//#include "SendCoordinate.h"

// #include "f_util.h"
// #include "ff.h"
// #include "rtc.h"
// #include "hw_config.h"

#define PI 3.14159265359
#define RAD_TO_DEG(rad) ((rad) * (180.0 / PI))

int fase = 1; //スタートフェーズ　1:待機，2:落下，3:分離，4:遠距離，　5:近距離

double goal_lat = 40.1426031670, goal_lon = 139.9876123330; //ゴール座標

//float calc_target_angle(GPS::Measurement_t gps, double heading);

float standerd_altitude;

#define BAUD_RATE 9600
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

#define RX_PIN_1 8
#define TX_PIN_1 9


int main(){
    stdio_init_all();
    // time_init();
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
    //uart1_init();

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
    absolute_time_t startTime, endTime;
    char *coordinate_char, *area_char;
    bool isTextReceived;
    int coordinate, area;

    // unsigned long long int count = 0;
    // sd_card_t *pSD = sd_get_by_num(0);
    // FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
    // if (FR_OK != fr) panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    // FIL fil;
    // const char* const filename = "CanSat_Logdata.txt";
    // fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
    // sleep_ms(100);
    // f_printf(&fil, "date, time, ");
    // sleep_ms(10);
    // f_printf(&fil, "fase, ");
    // sleep_ms(10);
    // f_printf(&fil, "latitude, longitude, distance, ");
    // sleep_ms(10);
    // f_printf(&fil, "temperature, humidity, pressure, altitude, ");
    // sleep_ms(10);
    // f_printf(&fil, "accel_X, accel_Y, accel_Z, resultant_accel");
    // sleep_ms(10);
    // f_printf(&fil, "mag_X, mag_Y, mag_Z, ");
    // f_printf(&fil, "azimuth, derection, target_angle\n");
    // sleep_ms(10);
    // fr = f_close(&fil);
    //f_unmount(pSD->pcName);
    // bool switch_fase = false;    

    sleep_ms(1000);
    startTime = get_absolute_time();

    while(true){
        bme = myBME280.measure();
        gps = myGPS.measure();
        BNO055_accel_mag();

        // pSD = sd_get_by_num(0);
        // fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
        // if (FR_OK != fr) panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
        sleep_ms(10);

        float target_angle = gps.target_angle - heading; 

        if (target_angle < 0) {
            target_angle += 360.0;
        } else if (target_angle >= 360.0) {
            target_angle -= 360.0;
        }

        if(fase == 4){
            while(gps.lat == -1024){
                tight_loop_contents();
                sleep_ms(100);
                gps = myGPS.measure();
            }
        }

        // switch_fase = false;
        endTime = get_absolute_time();
 
        switch(fase){
            case 1:     //待機フェーズ
                if(resultant_accel > 20 || absolute_time_diff_us(startTime, endTime) > 6100000000){
                    // switch_fase = true;
                    fase = 2;
                    sleep_ms(100);
                    break;
                }else{
                    break;
                }

            case 2:     //落下フェーズ
                if(fabs(bme.altitude_2 - standerd_altitude) < 10){
                    // switch_fase = true;
                    fase = 3;
                    sleep_ms(100);
                }else{
                    break;
                }
            
            case 3:     //分離フェーズ
                sleep_ms(15000);
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
                    // switch_fase = true;
                    fase = 5;
                    //printf("GOAL\n");
                    //gpio_put(25, 1);
                    // fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
                    // f_printf(&fil, "%d-%d-%d, ", gps.year, gps.month, gps.day);
                    // sleep_ms(10);
                    // f_printf(&fil, "%d:%d:%d, ", gps.hour, gps.minute, gps.second);
                    // sleep_ms(10);
                    // f_printf(&fil, "GOAL!!\n");
                    // fr = f_close(&fil);
                    // while(true){
                    //     tight_loop_contents();
                    // }
                    //sleep_ms(100);
                    break;
                }
                if(target_angle < 45 || target_angle > 315){
                    printf("forward\n");
                    forward(1);
                    sleep_ms(3000);
                    forward(0.6);
                    sleep_ms(1000);
                    forward(0.2);
                    sleep_ms(1000);
                    forward(0);
                    sleep_ms(100);
                    break;
                }else if((target_angle) <135){
                    printf("right\n");
                    right(0.5);
                    sleep_ms(300);
                    right(0);
                    sleep_ms(100);
                    break;
                }else if(target_angle < 180){
                    printf("sharp_right\n");
                    right(0.5);
                    sleep_ms(600);
                    right(0);
                    sleep_ms(100);
                    break;
                }else if(target_angle > 225){
                    printf("left\n");
                    left(0.5);
                    sleep_ms(300);
                    left(0);
                    sleep_ms(100);
                    break;
                }else{
                    printf("sharp_left\n");
                    left(0.5);
                    sleep_ms(600);
                    left(0);
                    sleep_ms(100);
                    break;
                }

            case 5:     //近距離フェーズ
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
                    printf("cooedinate: %d, ", coordinate);
                    printf("area: %d\n", area);
                }else{
                    printf("not received\n");
                    break;
                }
                //SendCoordinate();
                if(isTextReceived){
                    if(area > 200000){
                        printf("3\n");
                        forward(1);
                        sleep_ms(1000);
                        forward(0);

                        printf("GOAL\n");
                        gpio_put(25, 1);

                        uart_puts(uart1, "GOAL!!");

                        // fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
                        // f_printf(&fil, "%d-%d-%d, ", gps.year, gps.month, gps.day);
                        // sleep_ms(10);
                        // f_printf(&fil, "%d:%d:%d, ", gps.hour, gps.minute, gps.second);
                        // sleep_ms(10);
                        // f_printf(&fil, "GOAL!!\n");
                        // fr = f_close(&fil);
                        // f_unmount(pSD->pcName);
                        while(true){
                            tight_loop_contents();
                        }
                    }else{
                        if(coordinate < 0){
                            printf("turn\n");
                            right(0.5);
                            sleep_ms(500);
                            right(0);
                            sleep_ms(100);
                            break;
                        }else if(coordinate < 215){
                            printf("left\n");
                            left(0.5);
                            sleep_ms(500);
                            left(0);
                            sleep_ms(100);
                            break;
                        }else if(coordinate < 425){
                            printf("forward\n");
                            forward(1);
                            sleep_ms(3000);
                            forward(0.6);
                            sleep_ms(1000);
                            forward(0.2);
                            sleep_ms(1000);
                            forward(0);
                            sleep_ms(100);
                            break;
                        }else{
                            printf("right\n");
                            right(0.5);
                            sleep_ms(500);
                            right(0);
                            sleep_ms(100);
                            break;
                        }
                  }
                }
        }

        sleep_ms(10);
        printf("date:%d-%d-%d\n", gps.year, gps.month, gps.day);
        printf("time:%d:%d:%d\n", gps.hour, gps.minute, gps.second);
        printf("lat:%.10f, lon:%.10f, distance:%10.6f\n",gps.lat, gps.lon, gps.target_distance );
        printf("temperature:%6.3f`C, humidity:%6.3f %c, pressure:%7.3fhPa, altitude:%6.3fm\n", bme.temperature, bme.humidity, bme.pressure, bme.altitude_2);
        printf("acceleration X:%6.2lf, Y:%6.2lf, Z: %6.2lf, resultant: %6.2lf, \n", f_accelX, f_accelY, f_accelZ, resultant_accel);
        printf("geomagnetism X: %6.2lf, Y: %6.2lf, Z: %6.2lf\n", f_magX, f_magY, f_magZ);
        printf("azimuth:%8.5f, derection:%.2lf, target_angle:%8.5f\n", gps.target_angle, heading, target_angle);
        printf("fase:%d\n", fase); 
        sleep_ms(100);

        char moji[100];
        sprintf(moji, "%d-%d-%d, %d:%d:%d, %d, %.10f, %.10f, %10.6f, %6.3f, %6.3f, %6.3f, %6.3f, %6.2lf, %6.2lf, %6.2lf, %6.2lf, %6.2lf, %6.2lf, %6.2lf, %8.5f, %.2lf, %8.5f", gps.year, gps.month, gps.day, gps.hour, gps.minute, gps.second, fase, gps.lat, gps.lon, gps.target_distance, bme.temperature, bme.humidity, bme.pressure, bme.altitude_2, f_accelX, f_accelY, f_accelZ, resultant_accel, f_magX, f_magY, f_magZ, gps.target_angle, heading, target_angle);
        uart_puts(uart1, moji);

        //if(switch_fase){
            // f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
            // sleep_ms(10);
            // f_printf(&fil, "%d-%d-%d, ", gps.year, gps.month, gps.day);
            // sleep_ms(10);
            // f_printf(&fil, "%d:%d:%d, ", gps.hour, gps.minute, gps.second);
            // sleep_ms(10);
            // f_printf(&fil, "%d, ", fase);
            // sleep_ms(10);
            // f_printf(&fil, "%.10f, %.10f, %10.6f, ",gps.lat, gps.lon, gps.target_distance );
            // sleep_ms(10);
            // f_printf(&fil, "%6.3f, %6.3f, %6.3f, %6.3f, ", bme.temperature, bme.humidity, bme.pressure, bme.altitude_2);
            // sleep_ms(10);
            // f_printf(&fil, "%6.2lf, %6.2lf, %6.2lf, %6.2lf, ", f_accelX, f_accelY, f_accelZ, resultant_accel);
            // sleep_ms(10);
            // f_printf(&fil, "%6.2lf, %6.2lf, %6.2lf, ", f_magX, f_magY, f_magZ);
            // sleep_ms(10);
            // f_printf(&fil, "%8.5f, %.2lf, %8.5f\n", gps.target_angle, heading, target_angle);
            // sleep_ms(10);
            // f_close(&fil);
            // sleep_ms(100);
       // }
    }
}

/*
float calc_target_angle(GPS::Measurement_t gps_value, double heading){
    double goal_vector_x = goal_lon - gps_value.lon;
    double goal_vector_y = goal_lat - gps_value.lat;
    double magnetic_vector_x = magX;
    double magnetic_vector_y = magY;
    double vector_x = goal_vector_x + magnetic_vector_x;
    double vector_y = goal_vector_y + magnetic_vector_y;

    double direction_angle = atan2(vector_y, vector_x);

    if (direction_angle < 0) {
        direction_angle += 2 * PI;
    }

    double target_angle = RAD_TO_DEG(direction_angle);

    if(target_angle > 360){
        target_angle -= 360;
    }
    return target_angle;
}
*/