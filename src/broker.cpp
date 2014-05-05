#include "broker.h"
#include "message.h"
#include "string_hash.h"
#include "logger.h"
#include <string>
#include <iostream>
#include <fstream>
#include <zmq.hpp>

DECLARE_LOGGER_NAMESPACE("broker")

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

    while (true) {
        Message message(socket);
        handleMessage(message);
        LOGGER_INFO("Now tracking " << clients_.size() << " clients.  RECV: " << recv_ << " SENT: " << sent_);
    }
}

// As we're a DEALER the format will be [ SENDER, '' ]
void Broker::handleMessage(const Message& message) {
    ++recv_;
    const std::string& client_id = message.frames().at(0);
    Client& client = clients_[client_id];
    if (client.state == ClientState::NEW) {
        client.id = client_id;
    }
    client.last_recv = std::chrono::steady_clock::now();

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
            for (const auto& name : body) {
                Topic& topic = topics_[name];
                topic.subscribers.insert(&client);
            }
            break;

        case "UNSUB"_hash:
            for (const auto& name : body) {
                Topic& topic = topics_[name];
                topic.subscribers.erase(&client);

                // TODO(richardc): consider doing this cleanup async if we're
                // dropping/creating the same topics often
                if (topic.subscribers.size() == 0) {
                    topics_.erase(name);
                }
            }
            break;

        case "PUT"_hash: {
            std::string name = headers["TOPIC"];
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

    // Was an ACK requested?
    auto search = headers.find("ID");
    if (search != end(headers)) {
        sendMessage(client, { "OK", "ID", search->second });
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
    LOGGER_DEBUG("Loading privatekey from " << path)
    std::ifstream input(path);
    std::string key;
    input >> key;
    return key;
}
