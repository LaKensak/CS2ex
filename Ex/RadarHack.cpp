#include "RadarHack.hpp"
#include "Offsets.hpp"

RadarHack::RadarHack(Memory& mem) : memory(mem), initialized(false) {}

bool RadarHack::Initialize() {
    initialized = true;
    return true;
}

void RadarHack::Run() {
    if (!initialized || !Config::radarHack.enabled) return;

    uintptr_t localPlayerPtr = memory.Read<uintptr_t>(memory.GetClientBase() + offsets::signatures::dwLocalPlayerController);
    if (!localPlayerPtr) return;

    Entity localPlayer(&memory, localPlayerPtr);
    if (!localPlayer.IsValid() || !localPlayer.IsAlive()) return;

    for (int i = 1; i < 64; i++) {
        uintptr_t entityPtr = memory.Read<uintptr_t>(memory.GetClientBase() + offsets::signatures::dwEntityList + (i * 0x10));
        if (!entityPtr) continue;

        Entity entity(&memory, entityPtr);
        if (!entity.IsValid() || !entity.IsAlive() || entity.IsDormant()) continue;

        memory.Write<bool>(entityPtr + offsets::netvars::m_bSpotted, true);
    }
}
