#include <stdio.h>
#include <string.h>
#include <string>
#include <deque>
#include <cmath>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"


class GPS{
private:
    uart_inst_t *uart_hw;
    uint uart_id;
    uint tx_pin;
    uint rx_pin;
    uint baud_rate;

    const uint DATA_BITS = 8;
    const uint STOP_BITS = 1;
    const uart_parity_t PARITY = UART_PARITY_NONE;
    int UART_IRQ;
    int actual;
    const uint read_len = 800;

public:
    GPS(uint uart_id,
        uint tx_pin,
        uint rx_pin,
        uint baud_rate);
    
    struct Measurement_t{
        double lat             = -1024.0;
        double lon             = -1024.0;
        float altitude         = -1024.0F;
        float HDOP             = -1024.0F;
        float geoid_separation = -1024.0F;
        float velocity         = -1024.0F;
        int second             = -1024;
        int minute             = -1024;
        int hour               = -1024;
        int day                = -1024;
        int month              = -1024;
        int year               = -1024;
        double target_angle    = -1024.0;
        double target_distance = -1024.0;
    } measurement;

    Measurement_t measure();
private:
    std::deque<char> recv_copy;

    void output_time            (std::string&);
    void output_lat             (std::string&, std::string&);
    void output_lon             (std::string&, std::string&);
    void output_altitude        (std::string&);
    void output_HDOP            (std::string&);
    void output_geoid_separation(std::string&);
    void output_velocity        (std::string&);
    void output_date            (std::string&);

    void UTCtoJST_date();
    const int time_diff = +9;

public:
    void set_target(double, double);
private:
    double target_lat;
    double target_lon;
    const double R = 6369275.1; //地球の半径(地球楕円体の緯度40度の中心からの距離)
    void output_target();
};