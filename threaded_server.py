import socket
import threading
import socketserver
import sys
import json
import cryptography
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives import hashes

from cryptography.hazmat.primitives.asymmetric import padding

mutex = threading.Lock()

MESSAGE_QUEUE = []
CIPERTEXT_OUTPUT_FILE = "log.txt"

open_file = open("log.txt", 'w')
open_file.close()

config_file = sys.argv[1]
with open(config_file, "r") as read_file:
    data = json.load(read_file)
    read_file.close()

server_ip = data['server_ip']
server_port = data['server_port']
private_key_path = data['private_key_path']

with open(private_key_path, "rb") as key_file:
    private_key = serialization.load_pem_private_key(
        key_file.read(),
        password=None,

    )

def decrypt_message(ciphertext, private_key):
    plaintext = private_key.decrypt(
    ciphertext,
    padding.OAEP(
        mgf=padding.MGF1(algorithm=hashes.SHA1()),
        algorithm=hashes.SHA1(),
        label=None)
    )
    return plaintext

class ThreadedTCPRequestHandler(socketserver.BaseRequestHandler):
    def handle(self):
        while(1):
            #data = str(self.request.recv(1024), 'ascii')
            data = self.request.recv(1024)
            #if data == "exit()":
            #    print("leaving server")
            #    break
            # message = data.decode()
            # cur_thread = threading.current_thread()
            # Acquire the lock before output file opening file for writing
            mutex.acquire()
            try:
                ptext = decrypt_message(data, private_key)
                ctext_output = open("log.txt", 'a')
                ctext_output.write(f"{data}\n")
                ctext_output.close()
                #print(f"{data}")
            finally:
                response = bytes("recieved", 'ascii')
                self.request.sendall(response)
                #print(f"{data}\n")
                mutex.release()

        print("Server thread loop exited")
            # response = bytes("recieved", 'ascii')
            # self.request.sendall(response)


class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    pass

def client(ip, port, message):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((ip, port))
        sock.sendall(bytes(message, 'ascii'))
        response = str(sock.recv(1024), 'ascii')
        print("recieved: ()".format(response))

if __name__=="__main__":
    #HOST, PORT = "127.0.0.1", 51000
    
    # config_file = sys.argv[1]

    # with open(config_file, "r") as read_file:
    #     data = json.load(read_file)
    #     read_file.close()

    # server_ip = data['server_ip']
    # server_port = data['server_port']

    #print(f"python3 threaded_server.py {server_ip} {server_port}")


    HOST, PORT = server_ip, server_port

    # Start the server
    server = ThreadedTCPServer((HOST, PORT), ThreadedTCPRequestHandler)

    with server:
        ip, port =  server.server_address

        server_thread = threading.Thread(target=server.serve_forever)
        server_thread.daemon = True

        server_thread.start()
        print("Server loop running in thread:", server_thread.name)
        #client(ip, port, "HIIII")
        #while 1:
        #    2*2
        server_thread.join()

        server.shutdown()


