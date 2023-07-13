/*
このプログラムはGPSセンサ GT_902PMGGを読み取るためのものです．

データの受信に割り込み処理を利用することで，センサの値を直ちに返すことができます．
ただし，割り込み処理によってほかのプログラムに影響を与える可能性を否定できません．

このプログラムを作るにあたって，以下のプログラムを参考にしました．
    https://www.mztn.org/rpi/rpi53.html
    https://github.com/raspberrypi/pico-examples/blob/master/uart/uart_advanced/uart_advanced.c
    https://github.com/ms1963/bme280_spi/find/main
    https://orsj.org/wp-content/corsj/or60-12/or60_12_701.pdf
    https://www.logical-arts.jp/archives/18

長澤 悠太
*/

#include "GT_902PMGG_irq.hpp"

//読み取ったGPSの値をdequeに保存
namespace gps{
    std::deque<char> recv0;
    void read_raw0();
    std::deque<char> recv1;
    void read_raw1();
}

GPS::GPS(
    uint uart_id = 0, //uart0 or uart1
    uint tx_pin = PICO_DEFAULT_UART_TX_PIN,
    uint rx_pin = PICO_DEFAULT_UART_RX_PIN,
    uint baud_rate = 9600 //PICO_DEFAULT_UART_BAUD_RATEではない
){
    this->uart_id = uart_id;
    this->tx_pin = tx_pin;
    this->rx_pin = rx_pin;
    this->baud_rate = baud_rate;

    uart_hw = uart_id ? uart1 : uart0;

    uart_init(uart_hw, baud_rate);

    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);

    uart_set_hw_flow (uart_hw, false, false);

    uart_set_format(uart_hw, DATA_BITS, STOP_BITS, PARITY);

    (uart_id ? gps::recv1 : gps::recv0).resize(read_len);

    target_lat = 0;
    target_lon = 0;

    //ここから割り込み処理の設定

    // Turn off FIFO's - we want to do this character by character
    // uart_set_fifo_enabled(UART_ID, false);

    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    UART_IRQ = (uart_hw == uart0) ? UART0_IRQ : UART1_IRQ;
    
    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, uart_id ? gps::read_raw1 : gps::read_raw0);
    irq_set_enabled(UART_IRQ, true);

    // The call will return the actual baud rate selected, which will be as close as
    // possible to that requested
    int __unused actual = uart_set_baudrate(uart_hw, baud_rate);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(uart_hw, true, false);
    
}

//uart0のときにGPSの値をdequeに追加-->古い値を削除
inline void gps::read_raw0(){
    while (uart_is_readable(uart0)){
        gps::recv0.push_back(uart_getc(uart0));
        gps::recv0.pop_front();
    }
}
//uart1のときにGPSの値をdequeに追加-->古い値を削除
inline void gps::read_raw1(){
    while (uart_is_readable(uart1)){
        gps::recv1.push_back(uart_getc(uart1));
        gps::recv1.pop_front();
    }
}

//dequeに保存したGPSの値から数値を取り出す
GPS::Measurement_t GPS::measure(){
    std::deque<char> &recv = uart_id ? gps::recv1 : gps::recv0;
    if(recv.size() != read_len) recv.resize(read_len);
    recv_copy = recv;
    
    //------------------------------
    // この部分を削除することで，データを受信できなかった時にエラーの値-1024ではなく前回受信した値が出力されます．
    measurement.lat = measurement.lon = measurement.target_angle = measurement.target_distance = -1024.0;
    measurement.altitude = measurement.HDOP = measurement.geoid_separation = measurement.velocity = measurement.second = -1024.0F;
    measurement.minute = measurement.hour = measurement.day = measurement.month = measurement.year = -1024;
    //------------------------------

    int i = 0;
    int check_sum=0;
    while(i < read_len){
        check_sum = 0;
        std::deque<std::string> split_data(1);
        while(i<read_len && recv_copy.at(i++)!='$');  //最初の$まで移動
        for(; i<read_len && recv_copy.at(i)!='*'; i++){ //*になるまで次の文字を見ていく
            check_sum ^= recv_copy.at(i);
            if(recv_copy.at(i) == ','){
                split_data.push_back("");
                continue;
            }
            split_data.back() += recv_copy.at(i);
        }

        if(i >= read_len) break;

        std::string check_sum_str = "";

        i++;
        while(i<read_len && recv_copy.at(i)!='\r' && recv_copy.at(i)!='\n'){
            check_sum_str += recv_copy.at(i++);
        }
        
        //チェックサムで正常な値を取得できたか確認
        if(check_sum_str.size()==0 || check_sum != std::stoi(check_sum_str, nullptr, 16)){
            printf("\nfaild check_sum\n");
            continue;
        }

        //数値を出力するための変数に入れていく
        std::string nmea = split_data.at(0).substr(2);
        if      (nmea == "GGA"){
            if(split_data.at(7) != "00"){
                output_time(split_data.at(1));
                output_lat(split_data.at(2), split_data.at(3));
                output_lon(split_data.at(4), split_data.at(5));
                output_HDOP(split_data.at(8));
                output_altitude(split_data.at(9));
                output_geoid_separation(split_data.at(11));
            }
        }else if(nmea == "GLL"){
            if(split_data.at(6)!="V" && split_data.at(7)!="N"){
                output_lat(split_data.at(1), split_data.at(2));
                output_lon(split_data.at(3), split_data.at(4));
                output_time(split_data.at(5));
            }
        }else if(nmea == "RMC"){
            if(split_data.at(2)!="V" && split_data.at(12)!="N"){
                output_time(split_data.at(1));
                output_lat(split_data.at(3), split_data.at(4));
                output_lon(split_data.at(5), split_data.at(6));
                output_velocity(split_data.at(7));
                output_date(split_data.at(9));
            }
        }else if(nmea == "VTG"){
            if(split_data.at(9)!="N"){
                output_velocity(split_data.at(5));
            }
        }
    }

    UTCtoJST_date();
    output_target();

    return measurement;
}

//目標物の座標を設定
void GPS::set_target(double target_lat_, double target_lon_){
    target_lat = target_lat_;
    target_lon = target_lon_;
}

//時刻を出力するための変数に入れる
inline void GPS::output_time(std::string &value_str){
    if(value_str.size() == 9){
        measurement.hour = ((value_str.at(0)-'0')*10 + (value_str.at(1)-'0') + time_diff) % 24;
        measurement.minute = (value_str.at(2)-'0')*10 + value_str.at(3)-'0';
        measurement.second = (value_str.at(4)-'0')*10 + (value_str.at(5)-'0'); //小数形式で与えられるが，.00しか出力されないようなので小数部分は無視する
    }
}

//緯度を出力するための変数に入れる
inline void GPS::output_lat(std::string &value1_str, std::string &value2_str){
    if(value1_str.size() == 10 && value2_str != ""){    
        measurement.lat = ((value1_str.at(0)-'0')*10 + (value1_str.at(1)-'0') + ((value1_str.at(2)-'0')*10 + (value1_str.at(3)-'0') + (value1_str.at(5)-'0')*0.1 + (value1_str.at(6)-'0')*0.01 + (value1_str.at(7)-'0')*0.001 + (value1_str.at(8)-'0')*0.0001 + (value1_str.at(9)-'0')*0.00001)/60.0) * ((value2_str=="N")?1:-1);
    }
}

//経度を出力するための変数に入れる
inline void GPS::output_lon(std::string &value1_str, std::string &value2_str){
    if(value1_str.size() == 11 && value2_str != ""){    
        measurement.lon = ((value1_str.at(0)-'0')*100 + (value1_str.at(1)-'0')*10 + (value1_str.at(2)-'0') + ((value1_str.at(3)-'0')*10 + (value1_str.at(4)-'0') + (value1_str.at(6)-'0')*0.1 + (value1_str.at(7)-'0')*0.01 + (value1_str.at(8)-'0')*0.001 + (value1_str.at(9)-'0')*0.0001 + (value1_str.at(10)-'0')*0.00001)/60.0) * ((value2_str=="E")?1:-1);
    }
}

//高度を出力するための変数に入れる
inline void GPS::output_altitude(std::string &value_str){
    if(value_str != ""){
        measurement.altitude = std::stof(value_str);
    }
}

//HDOPを出力するための変数に入れる
inline void GPS::output_HDOP(std::string &value_str){
    if(value_str != ""){
        measurement.HDOP = std::stof(value_str);
    }
}

//ジオイド高を出力するための変数に入れる
inline void GPS::output_geoid_separation(std::string &value_str){
    if(value_str != ""){
        measurement.geoid_separation = std::stof(value_str);
    }
}

//対地速度を出力するための変数に入れる
inline void GPS::output_velocity(std::string &value_str){
    if(value_str != ""){
        measurement.velocity = std::stof(value_str) * 1852.0F / 3600.0F;
    }
}

//日付を出力するための変数に入れる
inline void GPS::output_date(std::string &value_str){
    if(value_str.size() == 6){
        measurement.day = (value_str.at(0)-'0')*10 + (value_str.at(1)-'0');
        measurement.month = (value_str.at(2)-'0')*10 + (value_str.at(3)-'0');
        measurement.year = 2000 + (value_str.at(4)-'0')*10 + (value_str.at(5)-'0');
    }
}

//目標物との角度と距離を出力するための変数に入れる
void GPS::output_target(){
    if(measurement.lat==-1024.0 || measurement.lon==-1024.0) return;

    double t_lat_rad = target_lat * M_PI / 180;
    double m_lat_rad = measurement.lat * M_PI / 180;
    double t_lon_rad = target_lon * M_PI / 180;
    double m_lon_rad = measurement.lon * M_PI / 180;

    double cos_n = sin(t_lat_rad)*sin(m_lat_rad) + cos(t_lat_rad)*cos(m_lat_rad)*cos(t_lon_rad-m_lon_rad);

    measurement.target_distance = R * acos(cos_n);

    //同じ二点では角度は定義されない
    if(measurement.lon==target_lon && measurement.lat==target_lat) return;
    
    measurement.target_angle = atan((cos(m_lat_rad)*cos(t_lat_rad)*sin(t_lon_rad-m_lon_rad))/(sin(t_lat_rad)-sin(m_lat_rad)*(cos_n))) * 180 / M_PI;
    //東経から西経などへの計算も正確にするため，場合分けをしている．
    if     (target_lon-measurement.lon > 180) measurement.target_angle += (measurement.target_angle < 0 ? 360 : 180);
    else if(target_lon-measurement.lon >   0) measurement.target_angle += (measurement.target_angle < 0 ? 180 :   0);
    else if(target_lon-measurement.lon >-180) measurement.target_angle += (measurement.target_angle < 0 ? 360 : 180);
    else                                      measurement.target_angle += (measurement.target_angle < 0 ? 180 :   0);
}


// 時差を考慮して日付を変更
void GPS::UTCtoJST_date(){
    if(measurement.hour!=-1024 && measurement.day!=-1024){
        if(measurement.hour < time_diff){
            if(measurement.day >= 28){
                if(measurement.month == -1024){
                    measurement.day = -1024;
                    return;
                }
                if(measurement.day <= 29){
                    if(measurement.month == 2){
                        measurement.month = 3;
                        measurement.day = 1;
                    }else{
                        measurement.day++;
                    }
                }else if(measurement.day == 30){
                    if(measurement.month==4 || measurement.month==6 || measurement.month==9 || measurement.month==11){
                        measurement.month++;
                        measurement.day = 1;
                    }else{
                        measurement.day++;
                    }
                }else{
                    if(measurement.month==12){
                        if(measurement.year!=-1024) measurement.year++;
                        measurement.month = measurement.day = 1;
                    }else{
                        measurement.month++;
                        measurement.day = 1;
                    }
                }
            }else{
                measurement.day++;
            }
        }
    }else{
        measurement.day = -1024;
    }
}