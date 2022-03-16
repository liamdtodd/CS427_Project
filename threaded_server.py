import socket
import threading
import socketserver
import sys
import json
import cryptography
import base64

from cryptography.hazmat.primitives import serialization, hashes
from cryptography.hazmat.primitives.asymmetric import padding

# Check if ciphertexts are sent in order, for AEAD.  Global variable
CHECK_TIMESTAMP = 0

mutex = threading.Lock()

MESSAGE_QUEUE = []
CIPERTEXT_OUTPUT_FILE = "log.txt"

decrypted_ctxts = []

open_file = open("log.txt", 'wb')
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
        password=None
    )

# Wrapper function for decrypt
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
            # Get data from the socket
            self.data = self.request.recv(1024).strip()
            if self.data == b'':
                continue
            #print(repr(self.data))

            #Check if the data has been decrypted.
            if self.data in decrypted_ctxts:
                print("Replayed ciphertext: ", end='')
            else:
                decrypted_ctxts.append(self.data)

            mutex.acquire()
            try:
                ptext = decrypt_message(self.data, private_key)
                ctext_output = open("log.txt", 'ab')
                ctext_output.write(self.data)
                ctext_output.close()
                plaintext_string = str(ptext, encoding='ascii')
                
                strings = plaintext_string.split('\n')
                
                plaintext_string = strings[0]
                timestamp = int(strings[1].rstrip('\x00'), 10)
                
                global CHECK_TIMESTAMP
                if timestamp > CHECK_TIMESTAMP:
                    CHECK_TIMESTAMP = timestamp
                else:
                    print("Timestamps not equal, replay likely")

                
                print(plaintext_string, end='')
                print(f" [timestamp: {timestamp}]")
            finally:
                response = bytes("recieved", 'ascii')
                self.request.sendall(response)
                mutex.release()

        print("Server thread loop exited")
            # response = bytes("recieved", 'ascii')
            # self.request.sendall(response)


class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    pass


if __name__=="__main__":
    HOST, PORT = server_ip, server_port

    # Start the server
    server = ThreadedTCPServer((HOST, PORT), ThreadedTCPRequestHandler)

    with server:
        ip, port =  server.server_address

        server_thread = threading.Thread(target=server.serve_forever)
        server_thread.daemon = True

        server_thread.start()
        print("Server loop running in thread:", server_thread.name)
        server_thread.join()

        server.shutdown()


