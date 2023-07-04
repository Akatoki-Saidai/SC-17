#include "GT_902PMGG_irq.hpp"

int main() {
    stdio_init_all();
    sleep_ms(3000); // required for some OSses to make Pico visible

    GPS myGPS(
        0,    //uart0 or uart1
        1,   //tx_pin(GPIO_no)  RP_pico:TX----RX:GPS(Green)
        2,   //rx_pin(GPIO_no)  RP_pico:RX----TX:GPS(Orange)
        9600  //baud_rate
    );
    GPS::Measurement_t values;

    // 空読み込みをしてウォームアップ
    myGPS.measure();
    sleep_ms(1000);

    while (true) {
        printf("start\n");
        // 計測-->結果をvaluesに代入
        values = myGPS.measure();

        // 結果を出力
        printf("lat,lon         : %8.5f,%9.5f\n", values.lat, values.lon); //緯度,経度  北緯&東経が正, 南緯&西経が負
        printf("altitude        : %6.2f m\n",     values.altitude); //高度  開けた場所でないとあまり精度は良くない(+-20 m以上?)
        printf("geoid_separation: %6.2f m\n",     values.geoid_separation); //ジオイド高  普段使っている重力加速度基準の高度0 mと，地球を楕円体とみなした時の地表面の高さとの差（場所によって異なる）
        printf("velocity        : %8.5f m/s\n",   values.velocity);//速度
        printf("time            : %d:%d:%d\n",    values.hour, values.minute, values.second); //時刻
        printf("date            : %d-%d-%d\n",    values.year, values.month, values.day); //日付
        //読み取れなかったデータは-1024が出力される．(前回読み取った値がもう一度出力されるように変更可)

        printf("\n\n");
        sleep_ms(500);
    }
}