import sys
import zmq

port = "5556"
if len(sys.argv) > 1:
    port =  sys.argv[1]
    int(port)
context = zmq.Context()
socket = context.socket(zmq.SUB)
socket.connect("tcp://127.0.0.1:%s" % port)
topic = "chess_board"
socket.setsockopt_string(zmq.SUBSCRIBE, topic)

while True:

	string = socket.recv().decode()
	print(string[len(topic) + 1:])
	print()

