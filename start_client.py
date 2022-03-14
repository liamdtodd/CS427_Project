import json
import subprocess
import sys

# Set the name of the config file as the second argument
config_file = sys.argv[1]

with open(config_file, "r") as read_file:
    data = json.load(read_file)
    read_file.close()
print(data)

hostname = data['hostname']
host_port = data['host_port']
username = data['username']
# client_port = data['client_port']
# client_ip = data['client_ip']

#client_args = [hostname, host_port, username]
#server_args = [client_ip, client_port, "&"]

subprocess.run(["./client", hostname, host_port, username])
# subprocess.run("python3", client_ip, client_port, "&")


