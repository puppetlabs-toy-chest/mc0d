#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <string>
#include <vector>
#include <zmq.hpp>

namespace mc0 {

/// A multi-frame message type we can print and send
class Message {
  public:
    Message() {}
    explicit Message(zmq::socket_t& socket);
    explicit Message(std::vector<std::string> frames) : frames_(frames) {}
    void send(zmq::socket_t& socket) const;
    std::string inspect() const;
    const std::vector<std::string>& frames() const;
  private:
    std::vector<std::string> frames_;
};

}  // namespace mc0

#endif
