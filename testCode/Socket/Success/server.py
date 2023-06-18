import socket

class TCP():
    def __init__(self,host,port):
        serversock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        serversock.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
        serversock.bind((host,port)) #IPとPORTを指定してバインドします
        serversock.listen(1) #接続の待ち受けをします（キューの最大数を指定）
        self.OK = "OK"
        print('Waiting for connections...')
        self.clientsock, self.client_address = serversock.accept() #接続されればデータを格納
        print("success connect")

    def communicate(self):
        try:
            rcvmsg = self.clientsock.recv(512).decode()
            print(f'Received : {rcvmsg}')
            print(f'Send : {self.OK}')
            self.clientsock.sendall(self.OK.encode())
        except BrokenPipeError:
           pass

if __name__ == "__main__":
    host = "172.18.176.1"
    port = 6969
    comm = TCP(host,port)
    comm.communicate()
    comm.communicate()
    comm.clientsock.close()
