#ifndef BROKER_H_
#define BROKER_H_

#include <map>
#include <string>
#include <zmq.hpp>
#include "client.h"
#include "topic.h"
#include "message.h"

class Broker {
  public:
    Broker(std::string bind, std::string private_key) :
        bind_(bind), private_key_(private_key) {}

    void main_loop();

  private:
    void handleMessage(const Message& message);
    void sendMessage(Client &client, std::vector<std::string> frames);
    std::string readPrivateKey(std::string path);
    std::map<std::string,Client> clients_;
    std::map<std::string,Topic> topics_;
    zmq::socket_t* socket_;
    size_t recv_ = 0;
    size_t sent_ = 0;
    std::string bind_;
    std::string private_key_;
};

#endif
