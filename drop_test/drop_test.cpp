#include <string>
#include "bme280.hpp"
#include "microSD.hpp"
#include "BNO055.hpp"

int main(){
    stdio_init_all();
    sleep_ms(3000);

    //BMEのセットアップ
    BME280 myBME280(
        0,          // I2C0 or I2C1
        4,          // SDA pin is GPIOx
        5,          // SCL pin is GPIOx
        0x76,       // SDO--GND:0x76 , SDO--VDD:0x77 (bme280のSDOピンをrasberryPIのどこに繋げるかでアドレスを選択)
        500 * 1000, // x Hz (500*1000 is 500 kHz)
        BME280::MODE::MODE_FORCED   // これは気にしなくてよい
    );
    BME280::Measurement_t bme_values;

    //BNOのセットアップ
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
    accel_init();
    uint8_t accel[6]; // Store data from the 6 acceleration registers
    int16_t accelX, accelY, accelZ; // Combined 3 axis data
    float f_accelX, f_accelY, f_accelZ; // Float type of acceleration data
    uint8_t val = 0x08; // Start register address
    sleep_ms(1000);

    // 空読み込みをしてウォームアップ
    myBME280.measure();
    i2c_write_blocking(I2C_PORT, addr, &val, 1, true);
    i2c_read_blocking(I2C_PORT, addr, accel, 6, false);
    sleep_ms(100);


    
    // 標高を計算するための基準点の (気圧hPa, 気温`C, 標高m) をセット
    // 開始位置を標高0mとする
    bme_values = myBME280.measure();
    myBME280.set_origin(bme_values.pressure, bme_values.temperature, 0);
    
    char *c_accelX, *c_accelY, *c_accelZ;
    write("BNO055 accelX, accelY, accelZ, BME280 altitude_1, altitude_2\n");
    while (true) {
        bme_values = myBME280.measure();
        i2c_write_blocking(I2C_PORT, addr, &val, 1, true);
        i2c_read_blocking(I2C_PORT, addr, accel, 6, false);

        //BNOの値を計算
        accelX = ((accel[1]<<8) | accel[0]);
        accelY = ((accel[3]<<8) | accel[2]);
        accelZ = ((accel[5]<<8) | accel[4]);

        write((std::to_string(accelX/100.0) + ',' + std::to_string(accelY/100.0) + ',' + std::to_string(accelZ/100.0) + ',' + std::to_string(bme_values.altitude_1) + ',' + std::to_string(bme_values.altitude_2) + '\n').c_str());
        
        sleep_ms(100);
    }

    
}