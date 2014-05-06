#include "broker.h"
#include "message.h"
#include "string_hash.h"
#include "logger.h"
#include "timer.h"
#include <string>
#include <thread>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <zmq.hpp>

namespace mc0 {

DECLARE_LOGGER_NAMESPACE("mc0.broker")

void Broker::main_loop() {
    zmq::context_t context;
    zmq::socket_t socket(context, ZMQ_ROUTER);
    socket_ = &socket;

    // key the curve
    int on = 1;
    LOGGER_DEBUG("ZMQ_CURVE_SERVER");
    socket.setsockopt(ZMQ_CURVE_SERVER, &on, sizeof(on));

    std::string private_key = readPrivateKey(private_key_);
    LOGGER_DEBUG("ZMQ_CURVE_SECRETKEY");
    socket.setsockopt(ZMQ_CURVE_SECRETKEY, private_key.data(), private_key.size());

    LOGGER_DEBUG("Binding socket to " << bind_)
    socket.bind(bind_.c_str());

    std::thread recv(&Broker::recvHeartbeatThread, this);
    std::thread send(&Broker::sendHeartbeatThread, this);

    while (true) {
        Message message(socket);
        Timer timer;
        handleMessage(message);
        LOGGER_INFO("Handled message in " << timer.elapsedSeconds() << " seconds."
                    << " Now tracking " << clients_.size() << " clients. "
                    << topics_.size() << " topics."
                    << " RECV: " << recv_
                    << " SENT: " << sent_);
    }
}

// As we're a DEALER the format will be [ SENDER, '' ]
void Broker::handleMessage(const Message& message) {
    ++recv_;
    const std::string& client_id = message.frames().at(0);
    std::shared_ptr<Client> client(findOrCreateClient(client_id));
    client->last_recv = std::chrono::steady_clock::now();

    LOGGER_INFO("handleMessage: client '" << client_id << "' message = " << message.frames().size());
    if (message.frames().size() < 3) {
        LOGGER_WARN("got short message from " << client_id);
        return;
    }
    const std::string& verb = message.frames().at(2);

    std::unordered_map<std::string,std::string> headers;
    std::vector<std::string> body;
    for (size_t i = 3; i < message.frames().size(); i = i + 2) {
        const std::string& key = message.frames().at(i);
        if (!key.size()) {
            auto start_from = begin(message.frames());
            std::advance(start_from, i + 1);
            body.insert(begin(body), start_from, end(message.frames()));
            break;
        }
        headers[key] = message.frames().at(i + 1);
    }

    LOGGER_INFO("handleMessage: verb " << verb);
    switch (hash_(verb.c_str())) {
        case "CONNECT"_hash:
            if (client->state == ClientState::NEW) {
                std::stringstream ss(headers["TTL"]);
                ss >> client->ttl;
            }
            else {
                sendMessage(client, {"ERROR", "Cannot CONNECT while CONNECTED"});
            }
            break;

        case "DISCONNECT"_hash:
            removeClient(client_id);
            break;

        case "SUB"_hash:
            for (const auto& name : body) {
                subscribeClient(client, name);
            }
            break;

        case "UNSUB"_hash:
            for (const auto& name : body) {
                unsubscribeClient(client, name);
            }
            break;

        case "PUT"_hash: {
            std::string name = headers["TOPIC"];
            std::unique_lock<std::mutex> state_lock(state_mutex_);
            Topic& topic = topics_[name];
            if (topic.subscribers.empty()) {
                // No need to dance if nobody's watching
                break;
            }

            std::vector<std::string> payload = { "MESSAGE" };
            for (const auto &kv : headers) {
                // copy every header but ID
                if (hash_(kv.first.c_str()) != "ID"_hash) {
                    payload.push_back(kv.first);
                    payload.push_back(kv.second);
                }
            }
            payload.push_back("");

            for (auto& frame : body) {
                payload.push_back(frame);
            }

            for (auto& subscriber : topic.subscribers) {
                sendMessage(subscriber, payload);
            }
            break;
        }

        case "NOOP"_hash:
            break;

        default:
            LOGGER_ERROR("Unknown command: " << verb)
            std::vector<std::string> payload = { "ERROR", "MESSAGE", "Unknown command verb '" + verb + "'"  };
            auto search = headers.find("ID");
            if (search != end(headers)) {
                payload.push_back("ID");
                payload.push_back(search->second);
            }
            sendMessage(client, payload);
            return;
    }

    // Do we need to manage the ttl?
    if (client->ttl) {
        client->next_recv_by = client->last_recv + std::chrono::milliseconds(int(double(client->ttl) * 1.05));
    }

    // Was an ACK requested?
    auto search = headers.find("ID");
    if (search != end(headers)) {
        sendMessage(client, { "OK", "ID", search->second });
    }
}

void Broker::sendMessage(std::shared_ptr<Client> client, std::vector<std::string> frames) {
    // Start with the [ DESTINATION, '' ] envelope
    std::vector<std::string> to_send = { client->id, "" };

    // Append the message we were given
    to_send.insert(to_send.end(), begin(frames), end(frames));

    Message message(to_send);
    message.send(*socket_);
    ++sent_;
    client->last_send = std::chrono::steady_clock::now();
    if (client->ttl) {
        client->next_send_by = client->last_send + std::chrono::milliseconds(int(double(client->ttl) * 0.95));
    }
}

std::string Broker::readPrivateKey(std::string path) {
    LOGGER_DEBUG("Loading privatekey from " << path)
    std::ifstream input(path);
    std::string key;
    input >> key;
    return key;
}

std::shared_ptr<Client> Broker::findOrCreateClient(const std::string& name) {
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    auto search = clients_.find(name);
    if (search == end(clients_)) {
        std::shared_ptr<Client> client(new Client);
        client->id = name;
        clients_[name] = client;
        return client;
    }
    return search->second;
}

void Broker::removeClient(const std::string& name) {
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    auto search = clients_.find(name);
    if (search == end(clients_)) {
        // we don't have it
        return;
    }
    for (auto& topic : topics_) {
        topic.second.subscribers.erase(search->second);
    }
    clients_.erase(search);
}

void Broker::subscribeClient(std::shared_ptr<Client> client, const std::string& topic_name) {
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    Topic& topic = topics_[topic_name];
    topic.subscribers.insert(client);
}

void Broker::unsubscribeClient(std::shared_ptr<Client> client, const std::string& topic_name) {
    std::unique_lock<std::mutex> state_lock(state_mutex_);
    auto search = topics_.find(topic_name);
    if (search == end(topics_)) {
        // unknown topic - maybe log?
        return;
    }

    search->second.subscribers.erase(client);

    // TODO(richardc): consider doing this cleanup async if we're
    // dropping/creating the same topics often
    if (search->second.subscribers.size() == 0) {
        topics_.erase(search);
    }
}

void Broker::recvHeartbeatThread() {
    while (true) {
        std::vector<std::shared_ptr<Client>> to_kill;
        {
            std::unique_lock<std::mutex> state_lock(state_mutex_);
            LOGGER_DEBUG("recvHeartbeatThread checking in")
            auto now = std::chrono::steady_clock::now();
            for (auto client : clients_) {
                if (client.second->ttl && now >= client.second->next_recv_by) {
                    to_kill.push_back(client.second);
                }
            }
        }
        for (auto client : to_kill) {
            LOGGER_DEBUG("removing idle client " << client->id)
            removeClient(client->id);
        }
        // TODO(richardc): next sleep should be based on next due recv
        usleep(1000000);
    }
}

void Broker::sendHeartbeatThread() {
    while (true) {
        {
            std::unique_lock<std::mutex> state_lock(state_mutex_);
            LOGGER_DEBUG("sendHeartbeatThread checking in")
            auto now = std::chrono::steady_clock::now();
            for (auto client : clients_) {
                if (client.second->ttl && now >= client.second->next_send_by) {
                    LOGGER_DEBUG("sending NOOP to " << client.second->id)
                    sendMessage(client.second, { "NOOP" });
                }
            }
        }
        // TODO(richardc): next sleep should be based on next due send
        usleep(1000000);
    }
}

}  // namespace mc0
