"""
course section:  CSCE 3600:Systems Programming- Minor Assignment 5 Sockets
date:2020-04-26
desc:server port is 3333,$python3 minor5server 3333 to start up the game server.
    Server will received from clients nad response sent back to the clients;
    Each move should be sent to both clients and a msg to the other player that is their trun;
    When the game ends,the server should send the board and notify each player of the results.
author : Andrew Yang 11229926 ay0123
"""
from socket import *

import os, sys
from select import select
# from config import conf


bufferSize = 1024
users = []
marks = ['X', 'O']
board = [
    [' ', ' ', ' '],
    [' ', ' ', ' '],
    [' ', ' ', ' ']
         ]


def show_board(readList):
    board_Data = ""
    board_Data += "\n board\n"
    board_Data += " 1 2 3\n"
    board_Data += " +-+-+-\n"
    board_Data += "A|"+board[0][0]+"|"+board[0][1]+"|"+board[0][2]+"\n"
    board_Data += " +-+-+-\n"
    board_Data += "B|"+board[1][0]+"|"+board[1][1]+"|"+board[1][2]+"\n"
    board_Data += " +-+-+-\n"
    board_Data += "C|"+board[2][0]+"|"+board[2][1]+"|"+board[2][2]+"\n"
    # print(board_Data)
    # for c in readList[2:]:
    #     if c is not r:
    #         c.send(data)

    # print("show_board len(readList) is "+ str(len(readList)))
    for idx,client in enumerate(readList):
        msgToClient = ""
        msgToClient += marks[idx]
        msgToClient += ">"+board_Data
        print(msgToClient)
        client.send(msgToClient.encode())


def is_cmd_valid(buff):
    return buff[0] == 'M' or buff[1] == 'R' or buff[2] == '?'


def gen_help():
    buff = ""
    buff += "\n?-Display this help\n"
    buff += "R-Resign\n"
    buff += "Move<R><C>-Move where <R> is a row A, B or C and <C> is a column 1,2 or 3\n"
    buff += "   Example Moves: MA1 MC3 MB1"
    return buff


def move(buff, idx):
    """
    return value:
        0:valid move
        1:invalid move
    """
    returnval = ""
    i = ord(buff[1]) - ord('A')
    j = int(buff[2]) - 1
    if i < 0 or i > 2 or j < 0 or j > 2:
        returnval += "Invalid Move"
        returnval += "  Move should be M<R><C> with no spaces\n"
        returnval += "  Example: MA1 or MB3"
        return -2,returnval

    if board[i][j] != ' ':
        returnval += "That spot is already taken"
        return -1,returnval
    board[i][j] = marks[idx]
    returnval += "move successfully in this step"
    return 0,returnval


def check_win(idx):
    """
    return value:
        0: win
        -1: not yet
    """
    conds = [
        [[0, 0], [0, 1], [0, 2]],
        [[1, 0], [1, 1], [1, 2]],
        [[2, 0], [2, 1], [2, 2]],
        [[0, 0], [1, 0], [2, 0]],
        [[0, 1], [1, 1], [2, 1]],
        [[0, 2], [1, 2], [2, 2]],
        [[0, 0], [1, 1], [2, 2]],
        [[0, 2], [1, 1], [2, 0]]
    ]
    mark = marks[idx]
    for conds_item in conds:
        cnt = 0
        for item in conds_item:
            if board[item[0]][item[1]] != mark:
                break
            cnt += 1
        if cnt >= 3:
            return 0
    return -1


def check_tie():
    """
    return value:
        0: tie
        -1: not yet
    """
    for board_item in board:
        for item in board_item:
            if item == ' ':
                return -1
    return 0


def play():
    pass


def test():
    print("test")
    print("test show board")
    show_board()
    print(board[0][0])
    for board_item in board:
        tmpStr = ''
        for item in board_item:
            if tmpStr == '':
                tmpStr += item
            else:
                tmpStr += " "+item
        print(tmpStr)
    pass


def validate(client):
    """
    validate the client name is valid
    :param client:
    :return: false if client is exists
    :return: true if client is new
    """
    name = client.recv(bufferSize).decode()
    if name in users:
        client.send(b'Username already exists')
        return False
    else:
        users.append(name)
        client.send(b'Welcome!')
        return True


def saveconn(client):
    dataSent = ''
    mark = 0
    infoConn = ''
    if len(users) == 0:
        mark = 0
        infoConn = "You are player " + str(mark) + " . Waiting on player O to connect"
    else:
        mark = 1
        infoConn = "You are player " + str(mark)
    print(marks[mark]+"<Connected\n")
    # print(infoConn)
    if len(users) == 0:
        users.append('X')
        dataSent = 'X'
    else:
        users.append('O')
        dataSent = 'O'
    print(marks[mark]+">"+infoConn+"\n")
    client.send(infoConn.encode())
    # print("user len is " + str(len(users))+" ->")
    # print(users)


def accept(server):
    """
    accept the request from the socket client
    :param server: tcp socket
    :return: null
    """
    readList = [server, sys.stdin]
    writelist = []
    exceptlist = []
    this_turn = 0
    can_be_turn = False
    game_end = False

    while True:
        if len(users) == 2 and game_end is False:
            show_board(readList[2:])
            msgToClient = "Your turn"
            print(marks[this_turn]+">Your turn\n")
            readList[2+this_turn].send(msgToClient.encode())

        rs, ws, xs = select(readList, writelist, exceptlist)
        for r in rs:
            if r is server:  # accept the request from the client
                conn, addr = server.accept();
                saveconn(client=conn)
                readList.append(conn)
                break
            elif r is sys.stdin:
                data = sys.stdin.readline()  # send to all client message from the server
                if data == 'close\n':  # whether contains enter
                    for c in readList[2:]:
                        c.send(data.encode())
                        readList.remove(c)
                        # c.close()
                    print("Game Ended")
                    server.close()
                    admin_close_chatroom = "game server close the game"
                    print(admin_close_chatroom)
                    sys.exit(0)  # chatroom is closed by admin
                else:
                    data = "game server" + ": " + data
                    for c in readList[2:]:
                        c.send(data.encode())  # send the input data by server to all others client
            else:
                data = r.recv(bufferSize)
                closeData = data.decode()
                if closeData == '':
                    print("client close")
                    readList.remove(r)
                    r.close()
                    if len(readList[2:]) == 0:
                        server.close()
                        sys.exit(0)
                else:
                    # print("can_be_turn = "+str(can_be_turn))
                    for idx, client in enumerate(readList[2:]):
                        if client is r:
                            msgFromClient = data.decode().replace("\n", "")
                            print(marks[idx]+"<"+msgFromClient+"\n")

                            """
                            command validity check
                            """
                            # print("len(msgFromClient) = "+str(len(msgFromClient)) +" /"+msgFromClient)
                            if len(msgFromClient) == 3 and is_cmd_valid(msgFromClient) == False:
                                can_be_turn = False
                                msgToClientInvalid = "Invalid command"
                                print(marks[idx]+"<"+msgToClientInvalid+"\n")
                                client.send(msgToClientInvalid.encode())
                                break #?

                            """
                            handle ? from the client
                            """
                            if msgFromClient[0] == '?':
                                can_be_turn = False
                                msgToClientGenHelp = gen_help()
                                print(marks[idx]+"<"+msgToClientGenHelp+"\n")
                                client.send(msgToClientGenHelp.encode())
                                break



                            """
                            handle is not your turn
                            """
                            if this_turn!=idx:
                                can_be_turn = False
                                msgToClientNotYourTurn = "It's not your turn"
                                print(marks[idx] + "<" + msgToClientNotYourTurn + "\n")
                                client.send(msgToClientNotYourTurn.encode())
                                break

                            """
                            handle resigned
                            """
                            if msgFromClient[0]=='R':
                                can_be_turn = True
                                msgToClientResigned = "You win, "+marks[idx]+" resigned\n"
                                print(msgToClientResigned)
                                readList[2+1-idx].send(msgToClientResigned.encode())
                                # continue
                                #end
                                msgToClientClose = "close\n"
                                for c in readList[2:]:
                                    c.send(msgToClientClose.encode())
                                    # readList.remove(c)
                                print("Game Ended")
                                game_end = True
                                # server.close()#server close
                                # os._exit(0)


                            """
                            handle others invalid command by default
                            """
                            if len(msgFromClient) != 3 and game_end is False:
                                can_be_turn = False
                                msgToClientOthersInvalid = "Invalid command"
                                print(marks[idx] + "<" + msgToClientOthersInvalid + "\n")
                                client.send(msgToClientOthersInvalid.encode())
                                break  # ?=


                            """
                            here come real command: move
                            """
                            if len(msgFromClient) == 3:
                                ret, returnval = move(msgFromClient, idx)
                                if ret < 0:
                                    can_be_turn = False
                                    msgToClientMove = returnval
                                    print(marks[idx] + "<" +msgToClientMove + "\n")
                                    client.send(msgToClientMove.encode())
                                else:
                                    can_be_turn = True

                            """
                            check if win
                            """
                            if check_win(idx) == 0: #if win
                                can_be_turn = True
                                show_board(readList[2:])
                                msgToClientWin = "You win"
                                print(marks[idx]+msgToClientWin+"\n")
                                client.send(msgToClientWin.encode())

                                msgToClientLose = "You lose"
                                print(marks[1-idx]+msgToClientLose+"\n")
                                readList[2+1-idx].send(msgToClientLose.encode())


                                # end
                                msgToClientClose = "close\n"
                                for c in readList[2:]:
                                    c.send(msgToClientClose.encode())
                                    # readList.remove(c)
                                print("Game Ended")
                                game_end = True
                                # server.close()  # server close
                                # os._exit(0)

                            """
                            check if tie
                            """
                            if check_tie()==0:
                                can_be_turn = True
                                show_board(readList[2:])
                                msgToClientTie = "Tie game"
                                print(marks[0]+msgToClientTie+"\n")
                                readList[2+0].send(msgToClientTie.encode())

                                print(marks[1] + msgToClientTie + "\n")
                                readList[2 + 1].send(msgToClientTie.encode())

                                # end
                                msgToClientClose = "close\n"
                                for c in readList[2:]:
                                    c.send(msgToClientClose.encode())
                                    # readList.remove(c)
                                print("Game Ended")
                                game_end = True
                                # server.close()  # server close
                                # os._exit(0)
                    if can_be_turn is True:
                        this_turn = 1 - this_turn


if __name__ == "__main__":
    """start point"""
    server = socket(AF_INET, SOCK_STREAM)
    argc = len(sys.argv)
    if argc !=2:
        print("Usage: "+sys.argv[0]+" port\n")
        sys.exit(1)
    port = sys.argv[1]
    hostname = '127.0.0.1'
    server_address = hostname, int(port)
    # print(server_address)
    server.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    server.bind(server_address)
    # print("Server bind port")
    server.listen(100)
    # print("server is listening")
    print("Network Server Starting")
    print("Waiting on Clients")
    accept(server)

    # test()

