import socket as s
from HTTP.core.config import ClientConfig as cc
from HTTP.core.config import GlobalConfig as gc


class Client:
    def __init__(self):
        self.socket = s.socket(s.AF_INET, s.SOCK_STREAM)
        self.data = "Hello, server"
        self.data_size = 1000
        self._connect() #connect to the running server
    def _connect(self) -> None:
        self.socket.connect((s.gethostname(), gc.port))
        self.socket.sendall(self.data.encode())
        server_msg = self.socket.recv(self.data_size).decode()
        print(server_msg)


