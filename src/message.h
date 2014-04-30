#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <string>
#include <vector>
#include <zmq.hpp>

// A multi-frame message type we can print and send

class Message {
  public:
    explicit Message(zmq::socket_t& socket);
    explicit Message(std::vector<std::string> frames) : frames_(frames) {}
    void send(zmq::socket_t& socket) const;
    std::string frame(int index) const;
  private:
    std::vector<std::string> frames_;
};

#endif
