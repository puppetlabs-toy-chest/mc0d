#include "torture.h"
#include "logger.h"

DECLARE_LOGGER_NAMESPACE("torture.main")

int main() {
    Logger log;
    LOGGER_INFO("Starting torture suite")
    Torture torture("tcp://*:61616", "middleware.public");
    torture.main_loop();
    return 0;
}
