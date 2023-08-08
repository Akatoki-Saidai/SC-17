# GPS (GT_902PMGG) の読み取り　別バージョン
このプログラムは Raspberry pi pico で GPSセンサ GT_902PMGGを読み取るためのものです．
通信方式はUARTを使用しています

使用する際は GT_902PMGG_blocking.hpp, GT_902PMGG_blocking.cpp の二つををコピーして下さい．そのうえで，CMakeLists の add_executable に  GT_902PMGG_blocking.cpp を追加してください．

受信用の関数を呼び出してから受信しようとするため，受信には数秒の時間がかかります．
動作未確認です．