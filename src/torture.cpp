#include "torture.h"
#include "message.h"
#include "logger.h"
#include <vector>
#include <thread>
#include <unistd.h>
#include <zmq.hpp>
#include <zmq_utils.h>

DECLARE_LOGGER_NAMESPACE("torture.main")

void Torture::main_loop() {
    std::vector<std::thread> workers;
    for (int i = 0; i < 50; ++i) {
        workers.push_back(std::thread(&Torture::chattyClient, this, i));
    }

    for (auto &thread : workers) {
        thread.join();
    }
}

// each thread gets its own context and socket, because it's pretending to be
// a new host
void Torture::chattyClient(int id) {
    const std::string identity("chattyClient " + std::to_string(id));
    LOGGER_INFO(identity << " starting");
    char public_key[41];
    char secret_key[41];
    zmq_curve_keypair(public_key, secret_key);

    zmq::context_t context;
    zmq::socket_t socket(context, ZMQ_DEALER);
    socket.setsockopt(ZMQ_CURVE_SERVERKEY, public_key_.data(), public_key_.size());
    socket.setsockopt(ZMQ_CURVE_PUBLICKEY, public_key, 40);
    socket.setsockopt(ZMQ_CURVE_SECRETKEY, secret_key, 40);
    socket.setsockopt(ZMQ_IDENTITY, identity.data(), identity.size());

    LOGGER_INFO(identity << " connecting");
    socket.connect(target_.c_str());

    Message({ "", "CONNECT", "VERSION", "0.2", "TTL", "10000" }).send(socket);
    Message({ "", "SUB", identity }).send(socket);

    Message noop({ "", "NOOP" });
    // step 1, pump and dump a bunch of noops
    for (int i = 0; i < 1000; ++i) {
        noop.send(socket);
    }

    LOGGER_INFO(identity << " disconnecting");
    Message({"", "DISCONNECT"}).send(socket);

    LOGGER_INFO(identity << " exiting");
}
