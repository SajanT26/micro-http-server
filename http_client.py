#Sajan Thavarasa
#N01715615

from socket import *

server_name = "gaia.cs.umass.edu"
server_port = 80

client_socket = socket(AF_INET, SOCK_STREAM)

client_socket.connect((server_name, server_port))

request = (
    "GET /kurose_ross/interactive/index.php HTTP/1.1\r\n"
    "Host: gaia.cs.umass.edu\r\n"
    "Connection: close\r\n"
    "\r\n"
)

client_socket.sendall(request.encode())

while True:
    response = client_socket.recv(4096)
    if not response:
        break
    print(response.decode(errors="replace"))

client_socket.close()
