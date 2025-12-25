#include <exception>
#include <iostream>

#include "engine/engine.h"

int main() {
    try {
        const Engine engine = Engine::createEngine();
        engine.waitEventsUntilExit();
    } catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return -1;
    }
}