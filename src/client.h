#ifndef CLIENT_H_
#define CLIENT_H_

#include <chrono>
#include "message.h"

namespace mc0 {

enum class ClientState { NEW, CONNECTED };

class Client {
  public:
    std::string id;
    std::chrono::steady_clock::time_point last_recv;
    std::chrono::steady_clock::time_point last_send;
    ClientState state = ClientState::NEW;
};

}  // namespace mc0

#endif  // CLIENT_H_
