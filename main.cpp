#include <exception>
#include <iostream>
#include <ostream>

#include "engine.h"

int main() {
    try {
        const Engine engine = EngineLoader::createEngine();
        engine.waitEventsUntilExit();
    } catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return -1;
    }
}