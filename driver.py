import socket


#build a socket

serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

serversocket.bind((socket.gethostname(), 8000))


serversocket.listen(5)




