#pragma once
#include "Memory.hpp"
#include "Entity.hpp"
#include "Config.hpp"

class RadarHack {
private:
    Memory& memory;
    bool initialized;

public:
    RadarHack(Memory& mem);

    bool Initialize();
    void Run();
};
