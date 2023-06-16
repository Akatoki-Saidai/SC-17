import socket

def server(host,port):
    serversock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
    serversock.bind((host,port)) #IPとPORTを指定してバインドします
    serversock.listen(1) #接続の待ち受けをします（キューの最大数を指定）
    OK = "OK"
    print('Waiting for connections...')
    clientsock, client_address = serversock.accept() #接続されればデータを格納
    print("success connect")
    while True:
        try:
            rcvmsg = clientsock.recv(512).decode()
            if(rcvmsg == ""):
                continue
            print(f'Received : {rcvmsg}')
            print(f'Send : {OK}')
            clientsock.sendall(OK.encode())
        except BrokenPipeError:
           pass
    clientsock.close()

if __name__ == "__main__":
    host = "127.0.0.1"
    port = 6969
    server(host,port)

