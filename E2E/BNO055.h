#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c1

static int addr = 0x28;

uint8_t accel[6]; // acceleration registers
uint8_t mag[6];   // magnetometer registers
uint8_t accel_val = 0x08; // Start register address for acceleration data
uint8_t mag_val = 0x0E;   // Start register address for magnetometer data
int16_t accelX, accelY, accelZ; 
int16_t magX, magY, magZ;
double f_accelX, f_accelY, f_accelZ, resultant_accel;
double f_magX, f_magY, f_magZ; 
float heading;

void BNO055_init(void){
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14);
    gpio_pull_up(15);

    sleep_ms(1000);
    uint8_t reg = 0x00;
    uint8_t chipID[1];
    i2c_write_blocking(I2C_PORT, addr, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, addr, chipID, 1, false);
    sleep_ms(10);

    while (chipID[0] != 0xA0) {
        printf("Chip ID Not Correct - Check Connection!");
        sleep_ms(5000);
    }

    uint8_t data[2];
    data[0] = 0x3F;
    data[1] = 0x40;
    i2c_write_blocking(I2C_PORT, addr, data, 2, true);
    sleep_ms(10);

    // Reset all interrupt status bits
    data[0] = 0x3F;
    data[1] = 0x01;
    i2c_write_blocking(I2C_PORT, addr, data, 2, true);
    sleep_ms(10);

    // Configure Power Mode
    data[0] = 0x3E;
    data[1] = 0x00;
    i2c_write_blocking(I2C_PORT, addr, data, 2, true);
    sleep_ms(10);

    // Default Axis Configuration
    data[0] = 0x41;
    data[1] = 0x24;
    i2c_write_blocking(I2C_PORT, addr, data, 2, true);
    sleep_ms(10);

    // Default Axis Signs
    data[0] = 0x42;
    data[1] = 0x00;
    i2c_write_blocking(I2C_PORT, addr, data, 2, true);
    sleep_ms(10);

    // Set units to m/s^2
    data[0] = 0x3B;
    data[1] = 0b0001000;
    i2c_write_blocking(I2C_PORT, addr, data, 2, true);
    sleep_ms(10);

    // Set operation to acceleration only
    data[0] = 0x3D;
    data[1] = 0x0C;
    i2c_write_blocking(I2C_PORT, addr, data, 2, true);
    sleep_ms(10);

    // Configure Power Mode for Magnetometer
    data[0] = 0x3E;
    data[1] = 0x0B;
    i2c_write_blocking(I2C_PORT, addr, data, 2, true);
    sleep_ms(10);

    // Set operation mode to MDOF (Magnetometer + Accelerometer)
    data[0] = 0x3D;
    data[1] = 0x0C;
    i2c_write_blocking(I2C_PORT, addr, data, 2, true);
    sleep_ms(100);
}

void BNO055_accel_mag(void){
    // Read acceleration data
    i2c_write_blocking(I2C_PORT, addr, &accel_val, 1, true);
    i2c_read_blocking(I2C_PORT, addr, accel, 6, false);
    accelX = ((accel[1] << 8) | accel[0]);
    accelY = ((accel[3] << 8) | accel[2]);
    accelZ = ((accel[5] << 8) | accel[4]);

    f_accelX = accelX / 100.00;
    f_accelY = accelY / 100.00;
    f_accelZ = accelZ / 100.00;

    // Read magnetometer data
    i2c_write_blocking(I2C_PORT, addr, &mag_val, 1, true);
    i2c_read_blocking(I2C_PORT, addr, mag, 6, false);
    magX = ((mag[1] << 8) | mag[0]);
    magY = ((mag[3] << 8) | mag[2]);
    magZ = ((mag[5] << 8) | mag[4]);

    f_magX = magX / 16.00;
    f_magY = magY / 16.00;
    f_magZ = magZ / 16.00;

    heading = atan2((double)magY, (double)magX) * (180.0 / M_PI);
    if (heading < 0) {
        heading += 360.0;
    }

    resultant_accel = sqrt(pow(f_accelX, 2) + pow(f_accelY, 2) + pow(f_accelZ, 2));
}