#pragma once
#include "Memory.hpp"
#include "Entity.hpp"
#include "Offsets.hpp"
#include "Config.hpp"
#include <Windows.h>

class Triggerbot {
private:
    Memory& memory;
    bool initialized;

public:
    Triggerbot(Memory& mem);

    bool Initialize();
    void Run();
};
