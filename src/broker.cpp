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
    Client client = clients_[client_id];
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
        case "UNSUB"_hash:
        case "PUT"_hash:
        case "NOOP"_hash:
            sendMessage(client, { "NOOP", "I'm slack "});
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
