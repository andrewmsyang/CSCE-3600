"""
course section:  CSCE 3600:Systems Programming- Minor Assignment 5 Sockets
date:2020-04-26
desc:
    Using python3 minor5client hostname port to run the client.
    Client can be able to process commands from the user and msg from the server in any order.
author : Andrew Yang 11229926 ay0123
"""

import os, sys
from socket import *
from select import select
# from config import conf

current_user = ""
server_address = ""
bufferSize = 1024


def repeatConnect(client):
    """
    judge is same user  in system
    :param client:
    :return:
    """
    global current_user
    # current_user = input(conf.username)

    # connect to the server, validate the name
    client.connect(server_address)
    connect_to = "connect to "
    # print(connect_to, server_address)
    # client.send(current_user.encode())
    data = client.recv(bufferSize)
    # if data.decode() == conf.user_already_exists:
    #     print("Failed to connect server %s\n"+conf.user_already_exists)
    #     return False
    # else:
    #     data = current_user + ": " + conf.user_enter_chatroom + "\n"  # send message that a new user enter the chatroom
    #     client.send(data.encode())
    #     return True
    current_msg_from_server = data.decode()
    print(current_msg_from_server)
    return True

def client_connect(client):
    """
    client connect to the server
    :param client:
    :return:
    """
    readList = [client, sys.stdin]
    writeList = []
    exceptionList = []
    global current_user

    while True:
        rs, ws, xs = select(readList, writeList, exceptionList)

        try:
            for r in rs:
                if r is client:
                    data = client.recv(bufferSize)
                    if data.decode() == 'close\n' or data.decode() == 'close':  # client exits the game
                        dataCloseClient = ''
                        client.send(dataCloseClient.encode())
                        client.close()
                        sys.exit(0)
                    else:
                        print(data.decode())
                elif r is sys.stdin:
                    data = sys.stdin.readline()
                    if data == 'exit\n':
                        dataCloseClient = ''
                        client.send(dataCloseClient.encode())
                        client.close()
                        sys.exit(0)
                    else:
                        client.send(data.encode())  # client send message
        except KeyboardInterrupt:
            closeMsg = ''
            client.send(closeMsg.encode())
            client.close()
            sys.exit(0)


if __name__ == "__main__":
    """start point"""
    client = socket(AF_INET, SOCK_STREAM)
    argc = len(sys.argv)
    if argc != 3:
        print("Usage: "+sys.argv[0]+" hostname port\n")
        sys.exit(1)
    hostname = sys.argv[1]
    port = sys.argv[2]
    # print(hostname+" "+port)
    server_address = hostname, int(port)
    # print(server_address)

    if repeatConnect(client):
        client_connect(client)

