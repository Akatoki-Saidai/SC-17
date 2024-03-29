# SC-17
![RP2040のピン配置(出典元:https://blog.artifactnoise.com/raspberrypi-piconodetashitowodu-ndemiru1-artifactnoise.html)](https://blog.artifactnoise.com/images/2022-01-05_012048.png)  
![Picoのピン配置(出典元:https://tanuki-bayashin.hatenablog.com/entry/2022/10/09/123554)](https://cdn-ak.f.st-hatena.com/images/fotolife/T/Tanuki_Bayashin/20221008/20221008232305.png)  
git コマンドを忘れたり、探したいときは参照   
https://qiita.com/kohga/items/dccf135b0af395f69144  
# プログラムの書き方
1.「センサ名 C言語 」や 「センサ名 Pico」などで検索  
2.エラーと戦う
* 他の人がプログラムを見て、どのような処理をしてるかわかるように、コメントを残そう
* 参考にしたサイトのURLをプログラムに残そう 　　 
* わからないことがあったら上級生に聞こう
# C言語　プログラミング　用語(簡単に。間違っている部分があったらごめんなさい)  
* インタープリター方式とコンパイル方式  
  インタープリター方式とはソースコードを１行ずつ機械語に解釈しながら実行していく方式。Pythonなどの言語はインタープリター方式で動いている。  
  コンパイル方式とはソースコードをすべて一気に機械語に翻訳して、実行する方式。C/C++などの言語はコンパイル方式で動いている。  
 （注)Javaなどはこれらの中間みたいな感じ。気になったら調べてみて

  
# CMakeLIstsの書き方
## Cmakeはなぜ必要?
* CMakeはC, C++ などの言語のビルドをコンパイラに依存せず自動化する  
　コンパイラといっても、複数種類があるため、それらの設定などの使い方を覚えるのは大変ということで、CMakeが使うコンパイラなどの指定をしてあげれば、自動でやってくれる。  
  

* CMakeLists.txt に書かれている内容を実行するのに必要な CMake の最低バージョンを明示する  
  cmake_minimum_required(VERSION 3.12)  

* ヘッダファイルをたくさん含む、モジュールを取り込む機能  
  include(pico_sdk_import.cmake)  

* プロジェクト名と使用する言語を指定している  
  project(project1 C CXX ASM)  

* 使用するCとC++のバージョン指定  
  set(CMAKE_C_STANDARD 11)  
  set(CMAKE_CXX_STANDARD 17)  

* モジュールの初期化  
  pico_sdk_init()  

* どのC言語のファイルを実行ファイル(機械語)に翻訳するか指定している。また、翻訳後の実行ファイル名も指定している  
  add_executable(test test.c)  

* リンクするライブラリを指定している  
* testに,pico_stdlibなどをリンクする指定  
  target_link_libraries(test pico_stdlib hardware_gpio hardware_pwm)  

* マイコンのペリフェラル(処理結果を入出力するポート)を指定している  
* 1で有効化、0で無効化  
* printfやcoutなどで出力したものをusbやuartで見ることができる  
* 同様に、scanfやcinで入力するのもそのペリフェラルで行われる  
  pico_enable_stdio_usb(test 1)  
  pico_enable_stdio_uart(test 0)  

* picoの実行ファイルの形式はuf2だから、uf2を出力する指定。()は出力するファイルの名前  
  pico_add_extra_outputs(test)  

  
# build中に行われていること(下記のプログラムをビルドすることを考える)  
  #include<stdio.h>    
  #define NUMBER 1    
  int main(void){  
    printf("%d:Hello World!",NUMBER);     
    return 0;  
  }  

  * 1.プリプロセス  
      #のついたプログラムを実行すること。  
      プログラムを読み解いていくために、必要な知識(どんなライブラリを使うかやどんな名前を定義しているか)をまず処理する。  
      この処理によってテンポラリーファイルというのが生み出される。  
  * 2.コンパイル  
      テンポラリーファイルをアセンブリ言語に変換すること。この流れに納得するうえで、CPUが実際に理解できる言葉(命令)は数十個しかないことを知らなければならない。(CPUによって戸数は変わってくる)  
      機械語の言葉(命令)とアセンブリ言語の言葉(命令)は１対１で対応している。01010100...(機械語)　ではわかりずらいため、01010100...などの塊に名前を付けたのがアセンブリ言語。  
      C言語で気軽に書いているif や int など、C言語上では１行ですんでいるようなプログラムは実際には、複数行のアセンブリ言語などで作られている。  
      そのため、機械語に翻訳する前にアセンブリ言語に翻訳を行い、これを実行するプログラムをコンパイラと呼んでいる。  
  * 3.アセンブル  
      アセンブリ言語で書かれたプログラムをオブジェクトコード(中間言語)に変化すること。オブジェクトコードとはいわゆる機械語であり、01010100...が書かれている。  　
      しかし、まだ、コンピュータが実行できる形ではなく、プログラミング言語と機械語の間ということで中間言語と呼ばれている。  　
  * 4.リンク  
      自分の書いたプログラムを中間言語に変換したが、そのプログラムにはライブラリの内容が含まれている。実際に実行できるように、自分の書いたプログラムとライブラリを結びつけるのがリンカ(リンクをするプログラム)の役目。 これによって、やっと実行できる形になる。  
## buildとmakeの違い  
* build  
    コンパイルとリンクを自動的に制御して実行形式ファイルを生成させる処理。  
* make  
    ユーザーが意図的にコンパイラリンカを制御して効率的に実行形式ファイルを生成させる処理。     
このサイト
https://dev.grapecity.co.jp/support/powernews/column/clang/003/page03.htm
が短くわかりやすく書いています。
# git コマンドの使い方
1.コマンドラインを開く(検索バーで cmd とうつ)  
2.git clone https://github.com/Akatoki-Saidai/SC-16.git と入力でフォルダがダウンロードされる  
# プログラムを作成、または変更したら
1.コマンドラインを開き、cd SC-16 と入力でフォルダを移動する  
2.git add .  と入力(1つのファイルだけアップロードしたかったら、git add ファイル名)   
3.git commit -m "何かメッセージ書く(何を作成、変更したか)"    
4.git push origin main とうつ  
* ほかの人が作業して、SC-16の中身が変更されたら、git pull で変更点を自分のパソコンに反映できる。
* add や commit , push の取り消し方は一番上のURLを参照   
# branch
### branchとは
git には、mainで開発しているプログラムに試験的に、機能を追加したいとき、別の作業場所を作れる。こうすることで、mainでの開発を妨げることなく、新しい機能の開発などができる。イメージつかなかったら、
https://git-scm.com/book/ja/v2/Git-%E3%81%AE%E3%83%96%E3%83%A9%E3%83%B3%E3%83%81%E6%A9%9F%E8%83%BD-%E3%83%96%E3%83%A9%E3%83%B3%E3%83%81%E3%81%A8%E3%81%AF  
を参照
### branchの使い方
1.コマンドラインを開いて、cd SC-16 と入力   
2.git branch ブランチ名 を入力して、ブランチの作成   
3.git checkout ブランチ名 で作成したブランチに切り替え  
4.あとは上記の使い方のように作業する  
* git branch で、どんなbranchがあって、今自分がどこのbranchで作業しているかわかる。
# エラー  
error: failed to push some refs to 'https://github.com/Akatoki-Saidai/SC-16.git' と表示されて、push できなかったら、上げたいファイルを他の場所に避難させた後に、git pull を行い、その後ファイルを元の場所に戻し、push を行う
