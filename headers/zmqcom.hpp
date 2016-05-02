#pragma once
#include <zmq.hpp>

class ZMQCom
{
private:
    zmq::context_t *context;
    zmq::socket_t *publisher;
public:
    ZMQCom(int port = 5556);
    ~ZMQCom();
    void send_board(std::string board);
};

