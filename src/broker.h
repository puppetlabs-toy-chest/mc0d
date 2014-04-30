#ifndef BROKER_H_
#define BROKER_H_

#include <string>
#include <zmq.hpp>

class Broker {
  public:
    Broker(std::string bind, std::string private_key) :
        bind_(bind), private_key_(private_key) {}

    void main_loop();

  private:
    std::string readPrivateKey(std::string path);

    std::string bind_;
    std::string private_key_;
};

#endif
