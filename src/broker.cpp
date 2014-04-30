#include "broker.h"
#include "message.h"
#include "string_hash.h"
#include <string>
#include <iostream>
#include <fstream>
#include <zmq.hpp>

void Broker::main_loop() {
    zmq::context_t context;
    zmq::socket_t socket(context, ZMQ_ROUTER);
    socket_ = &socket;

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
        handleMessage(message);
        std::cout << "Now tracking " << clients_.size() << " clients.  RECV: " << recv_ << " SENT: " << sent_ << std::endl;
    }
}

// As we're a DEALER the format will be [ SENDER, '' ]
void Broker::handleMessage(const Message& message) {
    ++recv_;
    std::string client_id = message.frame(0);
    Client& client = clients_[client_id];
    if (client.state == ClientState::NEW) {
        client.id = client_id;
    }
    client.last_recv = std::chrono::steady_clock::now();

    std::string verb = message.frame(2);
    switch (hash_(verb.c_str())) {
        case "CONNECT"_hash:
            if (client.state == ClientState::NEW) {
                // TODO(richardc): Parse TTL
            }
            else {
                sendMessage(client, {"ERROR", "Cannot CONNECT while CONNECTED"});
            }
            break;

        case "DISCONNECT"_hash:
            clients_.erase(client_id);
            break;

        case "SUB"_hash:
            for (size_t i = 3; i < message.size(); i++) {
                Topic& topic = topics_[message.frame(i)];
                topic.subscribers.insert(&client);
            }
            break;

        case "UNSUB"_hash:
            for (size_t i = 3; i < message.size(); i++) {
                std::string name(message.frame(i));
                Topic& topic = topics_[name];
                topic.subscribers.erase(&client);
                if (topic.subscribers.size() == 0) {
                    topics_.erase(name);
                }
            }
            break;

        case "PUT"_hash: {
            std::vector<std::string> payload = { "MESSAGE" };
            for (size_t i = 3; i < message.size(); i++) {
                payload.push_back(message.frame(i));
            }
            std::string name(message.frame(3));
            Topic& topic = topics_[name];
            for (auto& subscriber : topic.subscribers) {
                sendMessage(*subscriber, payload);
            }
            break;
        }

        case "NOOP"_hash:
            // TODO(richardc) - we shouldn't really send this, but that we do
            // means we keep up our end of the heart-beating, just simply by
            // being chatty
            sendMessage(client, { "NOOP" });
            break;

        default:
            sendMessage(client, {"ERROR", "Unknown command verb '" + verb + "'" });
    }
}

void Broker::sendMessage(Client &client, std::vector<std::string> frames) {
    // Start with the [ DESTINATION, '' ] envelope
    std::vector<std::string> to_send = { client.id, "" };

    // Append the message we were given
    to_send.insert(to_send.end(), begin(frames), end(frames));

    Message message(to_send);
    message.send(*socket_);
    ++sent_;
    client.last_send = std::chrono::steady_clock::now();
}

std::string Broker::readPrivateKey(std::string path) {
    std::ifstream input(path);
    std::string key;
    input >> key;
    return key;
}
