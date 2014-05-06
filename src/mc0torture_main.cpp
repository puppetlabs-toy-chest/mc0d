#include "torture.h"
#include "logger.h"

DECLARE_LOGGER_NAMESPACE("mc0torture.main")

int main() {
    mc0::Logger logger("");
    mc0::Torture torture("tcp://127.0.0.1:61616", "middleware.public");
    torture.main_loop();
    return 0;
}
