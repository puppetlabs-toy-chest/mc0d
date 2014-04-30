#include "broker.h"
#include "message.h"
#include <string>
#include <iostream>
#include <fstream>
#include <zmq.hpp>

void Broker::main_loop() {
    zmq::context_t context;
    zmq::socket_t socket(context, ZMQ_ROUTER);

    // key the curve
    int on = 1;
    std::cout << "CURVE_SERVER" << std::endl;

    socket.setsockopt(ZMQ_CURVE_SERVER, &on, sizeof(on));

    std::string private_key = readPrivateKey(private_key_);
    std::cout << "CURVE_KEY";
    socket.setsockopt(ZMQ_CURVE_SECRETKEY, private_key.data(), private_key.size());

    socket.bind(bind_.c_str());

    while (true) {
        Message message(socket);
        std::cout << message.frame(0) << " says " << message.frame(2) << std::endl;
        // C++11 compilers are Wizards, Harry
        Message reply({ message.frame(0), "", "Why", "hello", "there" });
        reply.send(socket);
    }
}

std::string Broker::readPrivateKey(std::string path) {
    std::ifstream input(path);
    std::string key;
    input >> key;
    return key;
}
