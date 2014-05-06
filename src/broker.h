#ifndef BROKER_H_
#define BROKER_H_

#include "client.h"
#include "topic.h"
#include "message.h"
#include <unordered_map>
#include <string>
#include <mutex>
#include <zmq.hpp>

namespace mc0 {

class Broker {
  public:
    Broker(std::string bind, std::string private_key) :
        bind_(bind), private_key_(private_key) {}

    void main_loop();

  private:
    void handleMessage(const Message& message);
    void sendMessage(std::shared_ptr<Client> client, std::vector<std::string> frames);
    void recvHeartbeatThread();
    void sendHeartbeatThread();

    std::shared_ptr<Client> findOrCreateClient(const std::string& name);
    void removeClient(const std::string& name);
    void subscribeClient(std::shared_ptr<Client> client, const std::string& topic);
    void unsubscribeClient(std::shared_ptr<Client> client, const std::string& topic);

    std::string readPrivateKey(std::string path);

    std::mutex state_mutex_;
    std::unordered_map<std::string,std::shared_ptr<Client>> clients_;
    std::unordered_map<std::string,Topic> topics_;

    zmq::socket_t* socket_;
    size_t recv_ = 0;
    size_t sent_ = 0;
    std::string bind_;
    std::string private_key_;
};

}  // namespace mc0

#endif
