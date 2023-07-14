#include "bme280.hpp"

int main() {
    stdio_init_all();
    sleep_ms(3000); // required for some OSses to make Pico visible

    BME280 myBME280(
        0,          // I2C0 or I2C1
        4,          // SDA pin is GPIOx
        5,          // SCL pin is GPIOx
        0x76,       // SDO-GND:0x76 , SDO-VDD:0x77 (bme280のSDOピンをrasberryPIのどこに繋げるかでアドレスを選択)
        500 * 1000, // x Hz (500*1000 is 500 kHz)
        BME280::MODE::MODE_FORCED   // FORCED_MODE to reduce energy consumption
    );
    BME280::Measurement_t values;

    // 空読み込みをしてウォームアップ
    myBME280.measure();
    sleep_ms(100);
    
    // 標高を計算するための基準点の (気圧hPa, 気温`C, 標高m) をセット
    myBME280.set_origin(1013, 30, 0);
    /*
    // 開始位置を標高0mとする例
    values = myBME280.measure();
    myBME280.set_origin(values.pressure, values.temperature, 0);
    */

    while (true) {
        // 計測-->結果をvaluesに代入
        values = myBME280.measure();

        // 結果を出力
        printf("temperature: %6.3f `C\n",   values.temperature);  //気温
        printf("humidity   : %6.3f %c\n",  values.humidity, '%'); //湿度
        printf("pressure   : %6.3f hPa\n", values.pressure);      //気圧
        printf("altitude   : %6.3f m\n",   values.altitude_1);    //標高  公式1  //標高は2バージョン作ってみました
        printf("altitude   : %6.3f m\n",   values.altitude_2);    //標高  公式2  //公式2は基準点の気温が不要です
        printf("\n\n");
        sleep_ms(2000);
    }
    /*
    ピンの接触不良が起きたときは
        湿度：0 %
    くらいになります．
    */
}