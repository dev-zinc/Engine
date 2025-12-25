#include "engine.h"

int main() {
    const Engine engine = EngineLoader::createEngine();

    engine.waitEventsUntilExit();
}