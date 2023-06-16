#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

void setup(const char *address,int port);
void server(const char *message);

static int sockfd;
static struct sockaddr_in addr;
void setup(const char *address,int port){
    // ソケット生成
    if( (sockfd = socket( AF_INET, SOCK_STREAM, 0) ) < 0 ) {
        perror( "socket" );
    }

    // 送信先アドレス・ポート番号設定
    addr.sin_family = AF_INET;
    addr.sin_port = htons( port );
    addr.sin_addr.s_addr = inet_addr( address );

    // サーバ接続
    connect( sockfd, (struct sockaddr *)&addr, sizeof( struct sockaddr_in ) );

}

void client(const char *message){
    // データ送信
    char send_str[512];
    char receive_str[10];

    sprintf( send_str, "%s", message );
    printf( "send:%s\n", message );
    if( send( sockfd, send_str, strlen(send_str), 0 ) < 0 ) {
        perror( "send" );
    }
    else {
         recv( sockfd, receive_str, 10, 0 );
        printf( "receive:%s\n", receive_str );
    }
    sleep( 1 );
    if(message == "q"){
    // ソケットクローズ
        close( sockfd );
    }
}
