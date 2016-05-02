#include "../headers/zmqcom.hpp"
#include <string>
#include <iostream>
ZMQCom::ZMQCom(int port)
{
    context = new zmq::context_t(1);
    publisher = new zmq::socket_t(*context, ZMQ_PUB);
    publisher->bind("tcp://*:" + std::to_string(port));
}

ZMQCom::~ZMQCom()
{
    delete publisher;
    delete context;
}
void ZMQCom::send_board(std::string board)
{
    std::string topic = "chess_board";
    zmq::message_t message(board.size() + topic.size()); //creae new message 1000 bytes.
    snprintf((char *) message.data(), 1000, "%s %s", topic.c_str(), board.c_str());
    std::cout << board << std::endl;
    publisher->send(message);
}
