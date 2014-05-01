#include "broker.h"

int main() {
    Broker broker("tcp://*:61616", "middleware.private");
    broker.main_loop();
    return 0;
}
