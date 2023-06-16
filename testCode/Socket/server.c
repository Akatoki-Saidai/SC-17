//参考サイト https://www.mathkuro.com/c-cpp/c-tcp-socket-sample/
//神サイト https://daeudaeu.com/socket/
//stdlib https://renoji.com/IT.php?Contents=Program_C/HeaderFile_StandardLibrary_stdlib.html
//Linux用
#include <stdio.h>
#include <time.h>
#include <unistd.h>//UNIX標準に関するヘッダファイル
#include <errno.h> //エラーを知らせてくれるヘッダファイル
#include <string.h>
#include <stdlib.h>//標準ライブラリに関するヘッダファイル

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 6969

enum { //enumは上から順に連番を振ってくれる
    OK,//OK = 0
    NG// NG = 1
};

int main(void){

    int ret_code = 0;
    char *buf;
    uint32_t buf_len = 0;

    int fd_accept = -1;  //接続受け付け用のfd
    int fd_other = -1; //sendやrecv用のfd
    //fdとはファイルディスクリプタの略
    ssize_t size = 0; //エラー情報とサイズを格納するsignedな整数型  成功すると読み込んだバイト数を返すが、関数がエラーで失敗すると-1を戻り値として返す
    int flags = 0;

    int response = -1;//0:OK , 1:NG

    //ソケットアドレス構造体
    struct sockaddr_in sin,sin_client;
    memset(&sin,0,sizeof(sin));//sinポインタが示す番地に0をsin構造体のバイトの大きさの数だけ書き込む　あっさりいえば、0で初期化
    memset(&sin_client,0,sizeof(sin_client));

    socklen_t socklen = sizeof(sin_client);

    fd_accept = socket(AF_INET,SOCK_STREAM,0);//AF_INETはIPv4を指定。INET_STREAMはシーケンス化された両方向バイト・ストリーム。0はTCPプロトコルを指定
    if(fd_accept == -1){//TCPソケット作成に失敗したとき
        printf("Failed to socket(errno=%d:%s)\n",errno,strerror(errno));
        return -1;
    }

    sin.sin_family = AF_INET;//インターネットドメイン(IPv4)
    sin.sin_addr.s_addr = INADDR_ANY;//すべてのアドレスから接続を受け入れる(=0.0.0.0)
    sin.sin_port = htons(SERVER_PORT);//接続を待ち受けるポート

    ret_code = bind(fd_accept,(const struct sockaddr *)&sin,sizeof(sin));//bind はソケットに IP アドレスやポート番号を設定する関数
    if(ret_code == -1){//bind失敗したとき
        printf("Failed to bind(errno=%d:%s)\n",errno,strerror(errno));
        close(fd_accept);
        return -1;
    }

    while(1){
        printf("accept waiting...\n");
        //接続受け処理。接続されるまで待機、
        fd_other = accept(fd_accept,(struct sockaddr *)&sin_client,&socklen);
        if(fd_other == -1){
            printf("Failed to accept(errno=%d:%s)",errno,strerror(errno));
            continue;
        }
    }
        //データの受信
        size = recv(fd_other,&buf_len,sizeof(buf_len),flags);
        //ワンちゃん、このエラー取り、必要ないかも
        if(size < sizeof(buf_len)){//用意しているバッファの数と受信したデータの大きさが一致しない場合
            printf("Failed to recv data size(errno=%d:%s)\n",errno,strerror(errno));
            response = NG;
            goto send_response;
        }

        printf("buf_len = %d\n",buf_len);

        buf = malloc(buf_len);//動的にメモリを確保する関数
        if(buf == NULL){//メモリを確保できなかった時
            printf("Failled to malloc");
            response = NG;
            goto send_response;
        }

        printf("received data:%s",buf);
        free(buf);//メモリ開放
        response = OK;

    send_response:
        //レスポンスの送信
        size = send(fd_other,&response,sizeof(response),flags);
        if(size < sizeof(response)){
            printf("Failed to send data(errno=%d:%s)\n",errno,strerror(errno));
        }
        else{
            printf("Success to send response\n");
        }

        // send/recv用のソケットを閉じてFDを-1に戻しておく
        close(fd_other);
        fd_other = -1;

        return 0;
}

