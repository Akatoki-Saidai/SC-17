#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#define PIN_PWM0 4
#define PIN_PWM1 2

//good web site https://qiita.com/keyyum/items/8cb419f5278e13b6db4d
//https://kirikoshokunin.hatenablog.com/entry/2021/04/18/221012
//https://qiita.com/keyyum/items/e38b5bfc0c16f8cbb462
void forward(double duty);
void back(double duty);
void right(double duty);
void left(double duty);

 //PWM'S channel is called ”slice”
static pwm_config pwm0_slice_config;//pwm_config is defined in hardware/gpio.h
static pwm_config pwm1_slice_config;

void main(void) {
    //To use USB
    stdio_init_all();

    gpio_set_function(PIN_PWM0,GPIO_FUNC_PWM);
    gpio_set_function(PIN_PWM1,GPIO_FUNC_PWM);

    //gpios are assigned pwm 
    uint pwm0_slice_num = pwm_gpio_to_slice_num(PIN_PWM0);
    uint pwm1_slice_num = pwm_gpio_to_slice_num(PIN_PWM1);
    //get default config  
    pwm0_slice_config = pwm_get_default_config();
    pwm1_slice_config = pwm_get_default_config();
    /* 位相補正：なし                           */
    /* 分周：1分周                              */
    /* カウントモード：フリーランニング         */
    /* 極性：通常                               */
    /* ラップ値(一周期分のカウント値)：0xFFFF   */

    //normal mode config
    //pwm_init(pwm0_slice_num,&pwm0_slice_config,true);
    //pwm_init(pwm1_slice_num,&pwm1_slice_config,true);
    pwm_set_enabled(pwm0_slice_num, true);
    pwm_set_enabled(pwm1_slice_num, true);
    
    //Loop
    while (true)
    {
        forward(1);
        sleep_ms(3000);
        forward(0.2);
        sleep_ms(3000);
        forward(0);
        sleep_ms(2000);
        back(1);
        sleep_ms(3000);
        back(0.2);
        sleep_ms(3000);
        back(0);
        sleep_ms(2000);

    }
}

void forward(double duty){
    //duty 50%
    pwm_set_gpio_level( PIN_PWM0, ( pwm0_slice_config.top * duty ) );
}

void back(double duty){
    pwm_set_gpio_level( PIN_PWM1, ( pwm1_slice_config.top * duty ) );
}