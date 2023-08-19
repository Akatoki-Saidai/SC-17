#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#define PIN_PWM0 18
#define PIN_PWM1 19
#define PIN_PWM2 20
#define PIN_PWM3 21

//good web site https://qiita.com/keyyum/items/8cb419f5278e13b6db4d
//https://kirikoshokunin.hatenablog.com/entry/2021/04/18/221012
//https://qiita.com/keyyum/items/e38b5bfc0c16f8cbb462

void forward(double duty);
void back(double duty);
void right(double duty);
void left(double duty);

//PWM'S channel is called ”slice”
//1
static pwm_config pwm0_slice_config;//pwm_config is defined in hardware/gpio.h
static pwm_config pwm1_slice_config;
//2
static pwm_config pwm2_slice_config;//pwm_config is defined in hardware/gpio.h
static pwm_config pwm3_slice_config;
void motor_init(void) {
    //1
    gpio_set_function(PIN_PWM0,GPIO_FUNC_PWM);
    gpio_set_function(PIN_PWM1,GPIO_FUNC_PWM);
    //2
    gpio_set_function(PIN_PWM2,GPIO_FUNC_PWM);
    gpio_set_function(PIN_PWM3,GPIO_FUNC_PWM);
    //gpios are assigned pwm 
    //1
    uint pwm0_slice_num = pwm_gpio_to_slice_num(PIN_PWM0);
    uint pwm1_slice_num = pwm_gpio_to_slice_num(PIN_PWM1);
    //2
    uint pwm2_slice_num = pwm_gpio_to_slice_num(PIN_PWM2);
    uint pwm3_slice_num = pwm_gpio_to_slice_num(PIN_PWM3);
    //get default config  
    //1
    pwm0_slice_config = pwm_get_default_config();
    pwm1_slice_config = pwm_get_default_config();
    //2
    pwm2_slice_config = pwm_get_default_config();
    pwm3_slice_config = pwm_get_default_config();
    /* 位相補正：なし                           */
    /* 分周：1分周                              */
    /* カウントモード：フリーランニング         */
    /* 極性：通常                               */
    /* ラップ値(一周期分のカウント値)：0xFFFF   */

    //normal mode config
    //pwm_init(pwm0_slice_num,&pwm0_slice_config,true);
    //pwm_init(pwm1_slice_num,&pwm1_slice_config,true);

    //1
    pwm_set_enabled(pwm0_slice_num, true);
    pwm_set_enabled(pwm1_slice_num, true);
    //2
    pwm_set_enabled(pwm2_slice_num, true);
    pwm_set_enabled(pwm3_slice_num, true);   
}    

void back(double duty){
    //duty 50%
    pwm_set_gpio_level( PIN_PWM0, ( pwm0_slice_config.top * duty ) );
    pwm_set_gpio_level( PIN_PWM2, ( pwm2_slice_config.top * duty ) );
}

void forward(double duty){
    pwm_set_gpio_level( PIN_PWM1, ( pwm1_slice_config.top * duty ) );
    pwm_set_gpio_level( PIN_PWM3, ( pwm3_slice_config.top * duty ) );
}

void right(double duty){
    pwm_set_gpio_level( PIN_PWM1, ( pwm0_slice_config.top * duty ) );
    pwm_set_gpio_level( PIN_PWM2, ( pwm2_slice_config.top * duty ) );
}

void left(double duty){
    pwm_set_gpio_level( PIN_PWM0, ( pwm0_slice_config.top * duty ) );
    pwm_set_gpio_level( PIN_PWM3, ( pwm2_slice_config.top * duty ) );
}