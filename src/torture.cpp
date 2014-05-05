#include "torture.h"
#include "message.h"
#include "logger.h"
#include "timer.h"
#include "string_hash.h"
#include <vector>
#include <thread>
#include <unistd.h>
#include <zmq.hpp>
#include <zmq_utils.h>

DECLARE_LOGGER_NAMESPACE("torture")

void Torture::main_loop() {
    std::vector<std::thread> workers;
    for (int i = 0; i < 50; ++i) {
        workers.push_back(std::thread(&Torture::chattyClient, this, i, 1000));
    }

    for (auto &thread : workers) {
        thread.join();
    }
}

// each thread gets its own context and socket, because it's pretending to be
// a new host
void Torture::chattyClient(int id, int count) {
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
    // assumes it'll be [ "", "OK", "ID", identity ]
    Message({"", "CONNECT", "ID", identity}).send(socket);
    Message ok(socket);
    if (hash_(ok.frames().at(1).c_str()) != "OK"_hash) {
        LOGGER_ERROR(identity << " didn't get OK for CONNECT");
        return;
    }
    if (identity.compare(ok.frames().at(3))) {
        LOGGER_ERROR(identity << " didn't get token for CONNECT");
        return;
    }
    LOGGER_INFO(identity << " connected");


    // step 1, send a bunch of noops, ensure they're acked
    Timer timer;
    for (int i = 0; i < count; ++i) {
        std::string idstr(identity + " " + std::to_string(i));
        Message noop({ "", "NOOP", "ID", idstr });
        noop.send(socket);
        Message ok(socket);

        // assumes it'll be [ "", "OK", "ID", idstr ]
        if (hash_(ok.frames().at(1).c_str()) != "OK"_hash) {
            LOGGER_ERROR(identity << " didn't get OK for " << i);
            return;
        }
        if (idstr.compare(ok.frames().at(3))) {
            LOGGER_ERROR(identity << " didn't get token for " << i);
            return;
        }
    }
    LOGGER_INFO(identity << " sent and acked " << count << " messages in " << timer.elapsedSeconds());

    Message({"", "DISCONNECT"}).send(socket);

    LOGGER_INFO(identity << " exiting");
}
