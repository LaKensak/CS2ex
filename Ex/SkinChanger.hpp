#pragma once
#include "Memory.hpp"
#include "Entity.hpp"
#include "Config.hpp"

class SkinChanger {
private:
    Memory& memory;
    bool initialized;

    int GetSkinForWeapon(int weaponID) const;
public:
    SkinChanger(Memory& mem);

    bool Initialize();
    void Run();
};
