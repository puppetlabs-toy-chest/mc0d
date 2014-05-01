#include "torture.h"

int main() {
    Torture torture("tcp://*:61616", "middleware.public");
    torture.main_loop();
    return 0;
}
