# GPS (GT_902PMGG) の読み取り
このプログラムは Raspberry pi pico で GPSセンサ GT_902PMGGを読み取るためのものです．
通信方式はUARTを使用しています

使用する際は GT_902PMGG_irq.hpp, GT_902PMGG_irq.cpp の二つををコピーして下さい．そのうえで，CMakeLists の add_executable に  GT_902PMGG_irq.cpp を追加してください．

データの受信に割り込み処理を利用することで，センサの値を直ちに返すことができます．
一方で，割り込み処理によってほかのプログラムに影響を与える可能性を否定できません．