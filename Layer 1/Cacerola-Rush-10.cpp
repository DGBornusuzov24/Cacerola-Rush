#include <cstdlib>
#include <ctime>

#include "Game.h"

int main() {
    srand((unsigned int)time(nullptr));   // a new random maze every time
    runApplication();
    return 0;
}
