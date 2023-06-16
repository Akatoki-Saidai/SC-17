import socket

# ソケットオブジェクトの生成
# socket.AF_INET: IPv4の利用
# socket.SOCK_STREAM: TCPの利用
def server():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # ホスト&ポートを指定
    my_computer = socket.gethostname()
    server.bind((my_computer, 6969))
    # キューの数を指定する
    server.listen(1)

    while True:
        # 接続の受信を開始
        conn, address = server.accept()
        try:
            # リクエスト内容を取得
            req = conn.recv(1024).decode()
            print(f"Connection: {address}")
            print(f"Request: {req}")
            # レスポンスする
            conn.send(bytes("wwwwwwwwwwwwww")) #clientに送信される
        except:
            print("error")
        finally:
            # 接続を終了
            conn.close()
if __name__ == '__main__':
    server()