#include "GT_902PMGG_irq.hpp"

int main() {
    stdio_init_all();
    sleep_ms(3000); // required for some OSses to make Pico visible

    GPS myGPS(
        0,    //uart0 or uart1
        16,   //tx_pin(GPIO_no)  RP_pico:TX----RX:GPS(Green)
        17,   //rx_pin(GPIO_no)  RP_pico:RX----TX:GPS(Orange)
        9600  //baud_rate
    );
    GPS::Measurement_t values;

    // 空読み込みをしてウォームアップ
    myGPS.measure();
    sleep_ms(10000);

    //目標物の座標をセット（角度と距離の計算に使用）
    myGPS.set_target(35.861283, 139.607515);
    

    while (true) {
        printf("start\n");
        // 計測-->結果をvaluesに代入
        values = myGPS.measure();

        // 結果を出力
        //一定時間内に読み取れなかったデータは-1024が出力される．(前回読み取った値がもう一度出力されるように変更可)
        printf("lat,lon         : %10.7f,%11.7f\n", values.lat, values.lon); //緯度,経度  北緯&東経が正, 南緯&西経が負
        printf("altitude        : %6.2f m\n",     values.altitude); //高度  開けた場所でないと精度は良くない  使わないほうがいい
        printf("HDOP            : %6.2f\n",       values.HDOP); //水平方向の精度低下
        printf("geoid_separation: %6.2f m\n",     values.geoid_separation); //ジオイド高  普段使っている高度0 mと，地球を楕円体とみなした時の地表面の高さとの差（場所によって異なる）
        printf("velocity        : %8.5f m/s\n",   values.velocity); //対地速度
        printf("time            : %d:%d:%d\n",    values.hour, values.minute, values.second); //時刻
        printf("date            : %d-%d-%d\n",    values.year, values.month, values.day); //日付
        printf("target_angle    : %8.5f °\n",     values.target_angle); //目標の座標への角度 北0°->東90°->南180°->西270°->北
        printf("target_distance : %8.5f m\n",     values.target_distance); //精度は有効数字三桁程度
        //一定時間内に読み取れなかったデータは-1024が出力される．(前回読み取った値がもう一度出力されるように変更可)
        
        printf("\n\n");
        sleep_ms(1000);
    }
}