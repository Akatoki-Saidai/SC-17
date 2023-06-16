import socket

host = "127.0.0.1"#アドレス
port = 6969#ポート
def client(host,port):
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #ソケットオブジェクトの作成

    client.connect((host, port)) #サーバーに接続します

    for i in range(10):#10回何か送る
        message = str(i)
        print('Send : %s' % message)
        client.send(message.encode()) #データを送信します
        response = client.recv(4096) #一度に受け取るデータのサイズ。レシーブは適当な2の累乗に(大きすぎるとダメ）
        print('Received: %s' % response.decode())#送られてきたデータをデコード

if __name__ == "__main__":
    client(host,port)
