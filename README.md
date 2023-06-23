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
# git コマンドの使い方
1.コマンドラインを開く(検索バーで cmd とうつ)  
2.git clone https://github.com/Akatoki-Saidai/SC-16.git と入力でフォルダがダウンロードされる  
# プログラムを作成、または変更したら
1.コマンドラインを開き、cd SC-16 と入力でフォルダを移動する  
2.git add .  と入力(1つのファイルだけアップロードしたかったら、git add ファイル名)   
3.git commit -f "何かメッセージ書く(何を作成、変更したか)"    
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
