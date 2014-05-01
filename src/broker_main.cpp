#include "broker.h"
#include "logger.h"

int main() {
    Logger logger;
    Broker broker("tcp://*:61616", "middleware.private");
    broker.main_loop();
    return 0;
}
