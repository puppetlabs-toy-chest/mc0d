#include "torture.h"
#include "logger.h"

DECLARE_LOGGER_NAMESPACE("torture.main")

int main() {
    Logger logger("");
    Torture torture("tcp://127.0.0.1:61616", "middleware.public");
    torture.main_loop();
    return 0;
}
