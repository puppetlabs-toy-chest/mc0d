#ifndef CLIENT_H_
#define CLIENT_H_

#include <chrono>
#include "message.h"

enum class ClientState { NEW, CONNECTED };

class Client {
  public:
    std::string id;
    std::chrono::steady_clock::time_point last_recv;
    std::chrono::steady_clock::time_point last_send;
    ClientState state = ClientState::NEW;
};

#endif  // CLIENT_H_
