import sys
import zmq
from serial import Serial
import time

class CommandSender():
    def __init__(self, tty, baud):
        self.serial = Serial()
        self.serial.port = tty
        self.serial.baudrate = baud
        self.serial.rtscts=True
        self.serial.dsrdtr=True
        self.serial.open()
        self.connected = True
        time.sleep(1)

    def sendCommand(self, command):
        commandstr = "SET BOARD " + command + "\n"
        self.serial.write(commandstr.encode("ascii"))


#Open zmq subscriber.
port = "5556"
context = zmq.Context()
socket = context.socket(zmq.SUB)
socket.connect("tcp://127.0.0.1:%s" % port)
topic = "chess_board"
socket.setsockopt_string(zmq.SUBSCRIBE, topic)

#Open serial port
seri = CommandSender(sys.argv[1], sys.argv[2])




last_board = ""

while True:
        string = socket.recv().decode()
        board = string[len(topic) + 1:]
        if(board is not last_board):
            print(board)
            print()
            seri.sendCommand(board)
            last_board = board
