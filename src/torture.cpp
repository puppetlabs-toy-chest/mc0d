#include "torture.h"
#include "logger.h"
#include <vector>
#include <thread>
#include <unistd.h>
#include <zmq.hpp>

DECLARE_LOGGER_NAMESPACE("torture.main")

void Torture::main_loop() {
    std::vector<std::thread> workers;
    for (int i = 0; i < 100; ++i) {
        workers.push_back(std::thread(&Torture::chattyClient, this, i));
    }

    for (auto &thread : workers) {
        thread.join();
    }
}

void Torture::chattyClient(int id) {
    LOGGER_INFO("chattyClient " << id << " starting");
    usleep(10000);
    LOGGER_INFO("chattyClient " << id << " exiting");
}
