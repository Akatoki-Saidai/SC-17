#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
/*参考サイト  https://qiita.com/fygar256/items/b96aebbd79b4cbf19c2f  ほぼこれそのまま
              http://www.fos.kuis.kyoto-u.ac.jp/le2soft/siryo-html/node13.html
*/
int main() {
    int sockfd;//fdとはファイルディスクリプタの略
    int client_sockfd;
    struct sockaddr_in addr;//IPアドレスやポート番号の情報を保持するための構造体
 
    socklen_t len = sizeof( struct sockaddr_in );//構造体のデータサイズ
    struct sockaddr_in from_addr;
 
    char buf[1024];//送信されたデータの一時保管場所
 
    // 受信バッファ初期化
    memset( buf, 0, sizeof( buf ) );//bufポインタが示す番地に0をbuf構造体のバイトの大きさの数だけ書き込む　あっさりいえば、0で初期化
 
    // ソケット生成
    if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {//生成失敗したとき。つまり return -1になったとき
        perror( "socket" );
    }
 
    // 待ち受け用IP・ポート番号設定
    addr.sin_family = AF_INET;//AF_INETはIPv4を指定
    addr.sin_port = htons( 6969 );//ポートを指定
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");//アドレスを指定
 
    // バインド
    if( bind( sockfd, (struct sockaddr *)&addr, sizeof( addr ) ) < 0 ) {//bind はソケットに IP アドレスやポート番号を設定する関数。設定失敗したらエラーを返している
        perror( "bind" );
    }
 
    printf("waiting for client's connection..\n");
    // 受信待ち
    if( listen( sockfd, SOMAXCONN ) < 0 ) {//ソケットを接続町に設定。SOMAXCONNは何個接続要求を受け付けるか
        perror( "listen" );
    }
 
    // クライアントからのコネクト要求待ち
    if( ( client_sockfd = accept( sockfd, (struct sockaddr *)&from_addr, &len ) ) < 0 ) {//要求失敗したら
        perror( "accept" );
    }
 
    // 受信
    int rsize;
    while( 1 ) {
        rsize = recv( client_sockfd, buf, sizeof( buf ), 0 );

        if ( rsize == 0 ) {//何も受信なし
            break;
        } else if ( rsize == -1 ) {//受信失敗
            perror( "recv" );
        } else {//成功
            printf( "receive:%s\n", buf );
            sleep( 1 );
 
            // 応答
            printf( "send:%s\n", buf );
            write( client_sockfd, buf, rsize );
        }
    }
 
    // ソケットクローズ
    close( client_sockfd );
    close( sockfd );
 
    return 0;
}


