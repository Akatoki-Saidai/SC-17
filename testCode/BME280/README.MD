# BME280 気温，気圧，湿度 の読み取り
これは Raspberry Pi pico で温湿度・気圧センサーBME280 を読み込むためのプログラムです．
通信方法はI2Cを使用しています．

使用する際は bme280.hpp, bme280.cpp の二つををコピーして下さい．そのうえで，CMakeLists の add_executable に  bme280.cpp を追加してください．