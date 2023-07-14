#pragma once

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"



/* The following compensation functions are required to convert from the raw ADC
data from the chip to something usable. Each chip has a different set of
compensation parameters stored on the chip at point of manufacture, which are
read from the chip at startup and used inthese routines.
*/

class BME280 {
public:
    enum MODE { MODE_SLEEP = 0b00,
                MODE_FORCED = 0b01,
                MODE_NORMAL = 0b11};
private:
    const uint READ_BIT = 0x80;
    int32_t     t_fine;
    //T1-T3は温度用
    uint16_t    dig_T1;
    int16_t     dig_T2, dig_T3;
    //P1-P9は気圧用
    uint16_t    dig_P1;
    int16_t     dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
    //H1-H6は湿度用
    uint8_t     dig_H1, dig_H3;
    int8_t      dig_H6;
    int16_t     dig_H2, dig_H4, dig_H5;
    int32_t     adc_T, adc_P, adc_H;

    float       pressure0;
    float       temperature0;
    float       altitude0;

    i2c_inst_t *i2c_hw;
    uint i2c_no;   // I2C0 or I2C1
    uint scl_pin;  // SCLのピンのGPIOの番号 I2C clock
    uint sda_pin;  // SDAののピンのGPIOの番号
    uint addr;     // SDO-GND:0x76 , SDO-VDD:0x77 (bme280のSDOピンをrasberryPIのどこに繋げるかでアドレスを選択)
    uint freq;     // 計測する頻度 (freq Hz)
    uint8_t buffer[26]; // storage for compensation parameters
    uint8_t chip_id;
    MODE mode;

struct MeasurementControl_t {
    // temperature oversampling
    // 000 = skipped
    // 001 = x1
    // 010 = x2
    // 011 = x4
    // 100 = x8
    // 101 and above = x16
    unsigned int osrs_t : 3; ///< temperature oversampling

    // pressure oversampling
    // 000 = skipped
    // 001 = x1
    // 010 = x2
    // 011 = x4
    // 100 = x8
    // 101 and above = x16
    unsigned int osrs_p : 3; ///< pressure oversampling

    // device mode
    // 00       = sleep
    // 01 or 10 = forced
    // 11       = normal
    unsigned int mode : 2; ///< device mode

    /// @return combined ctrl register
    unsigned int get() { return (osrs_t << 5) | (osrs_p << 2) | mode; }
}   measurement_reg; //!< measurement register object

public:
    // struct used for transmitting sensor values 
    // from the sensor to the program
    struct Measurement_t {
        float temperature; //気温
        float humidity;    //湿度
        float pressure;    //気圧
        float altitude_1;    //高度
        float altitude_2;    //高度
    } measurement;

    // 標高を計算する基準点とする 気圧，気温，標高をセット
    void set_origin(float _pressure0, float _temperature0, float _altitude0);


    /*
    Constructor has the following default values for params
    uint i2c_no    = 0, 
    uint sda_pin   = PICO_DEFAULT_I2C_SDA_PIN, 
    uint scl_pin   = PICO_DEFAULT_I2C_SCL_PIN, 
    uint addr      = 0x76,
    uint freq      = 500 * 1000,
    MODE mode      = MODE_NORMAL) {
    */
    BME280( uint i2c_no,
            uint sda_pin,
            uint scl_pin,
            uint addr,
            uint freq,
            MODE mode);


    // get sensor values from BME280
    Measurement_t measure();
    // get chip ID from sensor (=I2C address)
    uint8_t get_chipID();
 


private:
    // auxilliary functions
    int32_t     compensate_temp(int32_t adc_T);
    uint32_t    compensate_pressure(int32_t adc_P); 
    uint32_t    compensate_humidity(int32_t adc_H);
    void        bme280_read_raw(int32_t *humidity, int32_t *pressure, int32_t *temperature);
    void        write_register(uint8_t reg, uint8_t data);
    void        read_registers(uint8_t reg, uint8_t *buf, uint16_t len);
    /* This function reads the manufacturing assigned compensation parameters from the device */
    void        read_compensation_parameters(); 
};