import socket

def client(req):
    # socket.AF_INET: IPv4の利用
    # socket.SOCK_STREAM: TCPの利用
    ipv4 = socket.AF_INET
    TCP_PROTCOL = socket.SOCK_STREAM

    # ソケットオブジェクトの生成
    client = socket.socket(ipv4,TCP_PROTCOL)

    # ホスト&ポートを指定
    my_computer = socket.gethostname()
    client.connect((my_computer,6969))

    # 接続オプション
    # socket.SOL_SOCKET: ソケット通信
    # socket.SO_REUSEADDR: 待ち状態中のポートが存在してもbindする
    client.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # リクエストする
    client.send(req.encode("UTF-8"))
    #　serverからのレスポンス内容を取得
    msg = client.recv(1024)
    print(msg.decode("utf-8"))

if __name__ == '__main__':
    while True:
        client(input())