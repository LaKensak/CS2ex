#include "NoFlash.hpp"
#include "Offsets.hpp"

NoFlash::NoFlash(Memory& mem) : memory(mem), initialized(false) {}

bool NoFlash::Initialize() {
    initialized = true;
    return true;
}

void NoFlash::Run() {
    if (!initialized || !Config::noFlash.enabled) return;

    uintptr_t localPlayerPtr = memory.Read<uintptr_t>(memory.GetClientBase() + offsets::signatures::dwLocalPlayerController);
    if (!localPlayerPtr) return;

    Entity localPlayer(&memory, localPlayerPtr);
    if (!localPlayer.IsValid() || !localPlayer.IsAlive()) return;

    float flashDuration = localPlayer.GetFlashDuration();
    if (flashDuration > 0.0f) {
        float newDuration = flashDuration - Config::noFlash.reduction;
        if (newDuration < 0.0f) newDuration = 0.0f;

        memory.Write<float>(localPlayerPtr + offsets::netvars::m_flFlashDuration, newDuration);
    }
}
