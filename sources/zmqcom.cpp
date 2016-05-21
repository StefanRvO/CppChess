#include "../headers/zmqcom.hpp"
#include <string>
#include <iostream>
ZMQCom::ZMQCom(int port)
{
    context = new zmq::context_t(1);
    publisher = new zmq::socket_t(*context, ZMQ_PUB);
    publisher->bind(std::string("tcp://*:" + std::to_string(port)).c_str());
}

ZMQCom::~ZMQCom()
{
    delete publisher;
    delete context;
}
void ZMQCom::send_board(std::string board)
{
    std::string topic = "chess_board";
    zmq::message_t message(topic.size() + board.size() + 1); //creae new message 1000 bytes.
    snprintf((char *) message.data(), 1000, "%s %s", topic.c_str(), board.c_str());
    std::cout << (char *)message.data() << std::endl;
    publisher->send(message);
}
