#pragma once
#include "Memory.hpp"
#include "Entity.hpp"
#include "Config.hpp"

class NoFlash {
private:
    Memory& memory;
    bool initialized;

public:
    NoFlash(Memory& mem);

    bool Initialize();
    void Run();
};
