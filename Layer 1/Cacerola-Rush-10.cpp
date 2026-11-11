#include <cstdlib>
#include <ctime>

#include "presentation/Game.h"

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));   // a new random maze every run
    runApplication();
    return 0;
}
