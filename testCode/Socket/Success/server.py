import socket
host = "127.0.0.1"
port = 6969

serversock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serversock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
serversock.bind((host,port)) #IPとPORTを指定してバインド(設定)する
serversock.listen(10) #接続の待ち受け(接続数を指定）
OK = "OK"
print('Waiting for connections...')
clientsock, client_address = serversock.accept() #接続されればデータを格納
while True:
    try:
        rcvmsg = clientsock.recv(1024).decode()#受信したデータをデコード    
        print('Received : %s' % (rcvmsg))
        print('Send : %s' % OK)
        clientsock.sendall(OK.encode())#OKを送信
    except:
        print("Error")#エラー
clientsock.close()#接続終了

